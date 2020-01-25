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
}

/* Update the objects in the scene */
bool EditorScene::Update(double dt)
{
	LevelScene::Update(dt);

	ImGui::Begin(LevelScene::GetName().c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
	if (ImGui::Button("Add New Zone")) {
		ZoneDef* newZone = new ZoneDef();
		newZone->zoneBounds = new BoundingBox();
		GameObjectManager::AddObject(newZone->zoneBounds);
		level_zones.push_back(newZone);
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove Selected Zone")) {

	}
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Zones In Level", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (int x = 0; x < level_zones.size(); x++) {
			ImGui::RadioButton(("Zone " + std::to_string(x)).c_str(), &selectedEditZone, x);
		}
	}
	ImGui::Separator();
	if (ImGui::Button("Add New Model")) {
		
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove Selected Model")) {
		
	}
	if (ImGui::CollapsingHeader(("Zone " + std::to_string(selectedEditZone) + " Models").c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
		if (!(level_zones.size() <= selectedEditZone)) {
			for (int x = 0; x < level_zones.at(selectedEditZone)->models.size(); x++) {
				ImGui::RadioButton(level_zones.at(selectedEditZone)->models.at(x).modelName.c_str(), &selectedEditModel, x);
			}
		}
		else
		{
			selectedEditZone = 0;
		}
	}
	ImGui::End();

	if (level_zones.size() <= selectedEditZone) return true;
	if (level_zones.at(selectedEditZone)->loadedModels.size() <= selectedEditModel) return true;
	GameObject* objectToEdit = level_zones.at(selectedEditZone)->loadedModels.at(selectedEditModel);

	float* objectMatrix = &objectToEdit->GetWorldMatrix4X4().m[0][0];
	float* projMatrix = &dxutils.MatrixToFloat4x4(dxshared::mProjection).m[0][0];
	float* viewMatrix = &dxutils.MatrixToFloat4x4(dxshared::mView).m[0][0];

	ImGuizmo::DrawCube(viewMatrix, projMatrix, objectMatrix);

	if (ImGui::RadioButton("Translate", dxshared::mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		dxshared::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", dxshared::mCurrentGizmoOperation == ImGuizmo::ROTATE))
		dxshared::mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", dxshared::mCurrentGizmoOperation == ImGuizmo::SCALE))
		dxshared::mCurrentGizmoOperation = ImGuizmo::SCALE;

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(viewMatrix, projMatrix, dxshared::mCurrentGizmoOperation, dxshared::mCurrentGizmoMode, objectMatrix, NULL, NULL);

	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(objectMatrix, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation, 3);
	ImGui::InputFloat3("Rt", matrixRotation, 3);
	ImGui::InputFloat3("Sc", matrixScale, 3);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, objectMatrix);

	objectToEdit->SetWorldMatrix4X4(DirectX::XMFLOAT4X4(objectMatrix));

	return true;
}

/* Render the objects in the scene */
void EditorScene::Render(double dt)
{
	LevelScene::Render(dt);
}