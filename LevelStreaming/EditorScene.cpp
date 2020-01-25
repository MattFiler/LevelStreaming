#include "EditorScene.h"

/* Load config files for level */
EditorScene::EditorScene(std::string levelName, std::string levelPath, LevelType levelType) : LevelScene(levelName, levelPath, levelType)
{
	in_editor_mode = true;
}

/* Init the objects in the scene */
void EditorScene::Init()
{
	LevelScene::Init();

	//Force-load all zones at all times in editor
	allActiveModels.clear();
	allActiveZoneDummys.clear();
	for (int i = 0; i < level_zones.size(); i++) {
		if (!IsZoneLoaded(i)) {
			LoadZone(i);
		}
	}
}

/* Release the objects in the scene */
void EditorScene::Release()
{
	LevelScene::Release();

	for (int i = 0; i < allActiveZoneDummys.size(); i++) {
		delete allActiveZoneDummys.at(i);
	}
	allActiveZoneDummys.clear();
}

/* Update the objects in the scene */
bool EditorScene::Update(double dt)
{
	LevelScene::Update(dt);

	//Grab references to all loaded models once loading is complete
	if (zone_load_queue.size() == 0 && allActiveModels.size() == 0) {
		for (int i = 0; i < level_zones.size(); i++) {
			for (int x = 0; x < level_zones.at(i)->loadedModels.size(); x++) {
				allActiveZoneDummys.push_back(level_zones.at(i)->zoneBounds);
				allActiveModels.push_back(level_zones.at(i)->loadedModels.at(x));
			}
		}
	}

	//Editor UI
	ImGui::Begin(LevelScene::GetName().c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
	ImGui::RadioButton("Edit Zone Transforms", &editType, 0);
	ImGui::RadioButton("Edit Model Transforms", &editType, 1);
	ImGui::Separator();
	if (ImGui::Button("Add New Zone")) {
		BoundingBox* zoneDummy = new BoundingBox();
		GameObjectManager::AddObject(zoneDummy);
		allActiveZoneDummys.push_back(zoneDummy);
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove Selected Zone")) {
		allActiveZoneDummys.erase(allActiveZoneDummys.begin() + selectedEditZone);
	}
	if (ImGui::CollapsingHeader("Zones In Level", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (int x = 0; x < allActiveZoneDummys.size(); x++) {
			ImGui::RadioButton(("Zone " + std::to_string(x)).c_str(), &selectedEditZone, x);
		}
	}
	ImGui::Separator();
	if (ImGui::Button("Add New Model")) {
		
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove Selected Model")) {
		allActiveModels.erase(allActiveModels.begin() + selectedEditModel);
	}
	if (ImGui::CollapsingHeader("Models In Level", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (int x = 0; x < allActiveModels.size(); x++) {
			ImGui::RadioButton(std::to_string(x).c_str(), &selectedEditModel, x);
		}
	}
	ImGui::End();

	//Only continue if our requested edit object is valid
	GameObject* objectToEdit = nullptr;
	if (editType == 1)
	{
		if (allActiveModels.size() <= selectedEditModel) return true;
		objectToEdit = allActiveModels.at(selectedEditModel);
	}
	else
	{
		if (allActiveZoneDummys.size() <= selectedEditZone) return true;
		objectToEdit = allActiveZoneDummys.at(selectedEditZone);
	}

	//Get matrices as float arrays
	float* objectMatrix = &objectToEdit->GetWorldMatrix4X4().m[0][0];
	float* projMatrix = &dxutils.MatrixToFloat4x4(dxshared::mProjection).m[0][0];
	float* viewMatrix = &dxutils.MatrixToFloat4x4(dxshared::mView).m[0][0];

	//Draw a representation of a zone if required
	if (editType == 0) ImGuizmo::DrawCube(viewMatrix, projMatrix, objectMatrix);

	//Show options to swap between different transforms
	if (ImGui::RadioButton("Translate", dxshared::mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		dxshared::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", dxshared::mCurrentGizmoOperation == ImGuizmo::ROTATE))
		dxshared::mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", dxshared::mCurrentGizmoOperation == ImGuizmo::SCALE))
		dxshared::mCurrentGizmoOperation = ImGuizmo::SCALE;

	//Allow swap between local/world
	if (dxshared::mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", dxshared::mCurrentGizmoMode == ImGuizmo::LOCAL))
			dxshared::mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", dxshared::mCurrentGizmoMode == ImGuizmo::WORLD))
			dxshared::mCurrentGizmoMode = ImGuizmo::WORLD;
	}

	//Draw manipulation control
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(viewMatrix, projMatrix, dxshared::mCurrentGizmoOperation, dxshared::mCurrentGizmoMode, objectMatrix, NULL, NULL);

	//Set new transforms back
	objectToEdit->SetWorldMatrix4X4(DirectX::XMFLOAT4X4(objectMatrix));

	return true;
}

/* Render the objects in the scene */
void EditorScene::Render(double dt)
{
	LevelScene::Render(dt);
}