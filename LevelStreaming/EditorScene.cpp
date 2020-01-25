#include "EditorScene.h"

/* Load config files for level */
EditorScene::EditorScene(std::string levelName, std::string levelPath, LevelType levelType) : LevelScene(levelName, levelPath, levelType)
{
#if _DEBUG
	in_editor_mode = true;
#endif
}

/* Init the objects in the scene */
void EditorScene::Init()
{
	LevelScene::Init();

#if _DEBUG
	//Force-load all zones at all times in editor
	allActiveModels.clear();
	allActiveModelNames.clear();
	allActiveZoneDummys.clear();
	for (int i = 0; i < level_zones.size(); i++) {
		if (!IsZoneLoaded(i)) {
			LoadZone(i);
		}
	}
#endif
}

/* Release the objects in the scene */
void EditorScene::Release()
{
	LevelScene::Release();

#if _DEBUG
	for (int i = 0; i < allActiveZoneDummys.size(); i++) {
		delete allActiveZoneDummys.at(i);
	}
	allActiveZoneDummys.clear();
#endif
}

/* Update the objects in the scene */
bool EditorScene::Update(double dt)
{
	LevelScene::Update(dt);

#if _DEBUG
	//Grab references to all loaded models once loading is complete
	if (zone_load_queue.size() == 0 && allActiveModels.size() == 0) {
		for (int i = 0; i < level_zones.size(); i++) {
			allActiveZoneDummys.push_back(level_zones.at(i)->zoneBounds);
			for (int x = 0; x < level_zones.at(i)->loadedModels.size(); x++) {
				allActiveModelNames.push_back(level_zones.at(i)->models.at(x).modelName);
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
		zoneDummy->Create();
		GameObjectManager::AddObject(zoneDummy);
		allActiveZoneDummys.push_back(zoneDummy);
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove Selected Zone")) {
		allActiveZoneDummys.at(selectedEditZone)->Release();
		delete allActiveZoneDummys.at(selectedEditZone);
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
	ImGui::Separator();
	if (ImGui::Button("Save Level")) {
		commands_json_out.clear();
		std::vector<int> saved_models = std::vector<int>();

		for (int i = 0; i < allActiveZoneDummys.size(); i++) {
			commands_json_out["ZONES"][i]["BOUNDS"][0][0] = allActiveZoneDummys.at(i)->GetPosition().x - allActiveZoneDummys.at(i)->GetScale().x;
			commands_json_out["ZONES"][i]["BOUNDS"][0][1] = allActiveZoneDummys.at(i)->GetPosition().y - allActiveZoneDummys.at(i)->GetScale().y;
			commands_json_out["ZONES"][i]["BOUNDS"][0][2] = allActiveZoneDummys.at(i)->GetPosition().z - allActiveZoneDummys.at(i)->GetScale().z;
			commands_json_out["ZONES"][i]["BOUNDS"][1][0] = allActiveZoneDummys.at(i)->GetPosition().x + allActiveZoneDummys.at(i)->GetScale().x;
			commands_json_out["ZONES"][i]["BOUNDS"][1][1] = allActiveZoneDummys.at(i)->GetPosition().y + allActiveZoneDummys.at(i)->GetScale().y;
			commands_json_out["ZONES"][i]["BOUNDS"][1][2] = allActiveZoneDummys.at(i)->GetPosition().z + allActiveZoneDummys.at(i)->GetScale().z;

			int count = 0;
			for (int y = 0; y < allActiveModels.size(); y++) {
				if (allActiveZoneDummys.at(i)->ContainsPoint(allActiveModels.at(y)->GetPosition())) {
					commands_json_out["ZONES"][i]["CONTENT"][count]["MODEL"] = allActiveModelNames.at(y);
					commands_json_out["ZONES"][i]["CONTENT"][count]["PLACEMENT"]["POSITION"][0] = allActiveModels.at(y)->GetPosition().x;
					commands_json_out["ZONES"][i]["CONTENT"][count]["PLACEMENT"]["POSITION"][1] = allActiveModels.at(y)->GetPosition().y;
					commands_json_out["ZONES"][i]["CONTENT"][count]["PLACEMENT"]["POSITION"][2] = allActiveModels.at(y)->GetPosition().z;
					commands_json_out["ZONES"][i]["CONTENT"][count]["PLACEMENT"]["ROTATION"][0] = allActiveModels.at(y)->GetRotation().x;
					commands_json_out["ZONES"][i]["CONTENT"][count]["PLACEMENT"]["ROTATION"][1] = allActiveModels.at(y)->GetRotation().y;
					commands_json_out["ZONES"][i]["CONTENT"][count]["PLACEMENT"]["ROTATION"][2] = allActiveModels.at(y)->GetRotation().z;
					commands_json_out["ZONES"][i]["CONTENT"][count]["PLACEMENT"]["SCALE"][0] = allActiveModels.at(y)->GetScale().x;
					commands_json_out["ZONES"][i]["CONTENT"][count]["PLACEMENT"]["SCALE"][1] = allActiveModels.at(y)->GetScale().y;
					commands_json_out["ZONES"][i]["CONTENT"][count]["PLACEMENT"]["SCALE"][2] = allActiveModels.at(y)->GetScale().z;
					count++;

					bool to_add = true;
					for (int z = 0; z < saved_models.size(); z++) {
						if (saved_models.at(z) == y) {
							to_add = false;
						}
					}
					if (to_add) {
						saved_models.push_back(y);
					}
				}
			}
		}

		if (saved_models.size() != allActiveModels.size()) {
			Debug::Log("Failed to save " + std::to_string(allActiveModels.size() - saved_models.size()) + " models! Check zone placement."); //TODO: show a proper IMGUI popup here
		} else {
			commands_json_out["PLAYER_SPAWN"]["POSITION"][0] = 0;
			commands_json_out["PLAYER_SPAWN"]["POSITION"][1] = 0;
			commands_json_out["PLAYER_SPAWN"]["POSITION"][2] = 0;
			commands_json_out["PLAYER_SPAWN"]["ROTATION"][0] = 0;
			commands_json_out["PLAYER_SPAWN"]["ROTATION"][1] = 0;
			commands_json_out["PLAYER_SPAWN"]["ROTATION"][2] = 0;

			std::vector<uint8_t> bson = json::to_bson(commands_json_out);
			std::ofstream commands_json_file(level_path + "COMMANDS.BIN", std::ios::out | std::ios::binary);
			commands_json_file.write((char*)&bson[0], bson.size() * sizeof(uint8_t));
			commands_json_file.close();

			Debug::Log("Saved level!"); //TODO: show a proper IMGUI popup here
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

	//Show options to swap between different transforms
	if (ImGui::RadioButton("Translate", dxshared::mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		dxshared::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (editType == 1) {
		if (ImGui::RadioButton("Rotate", dxshared::mCurrentGizmoOperation == ImGuizmo::ROTATE))
			dxshared::mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
	}
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

	//Show current translations in UI
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(objectMatrix, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Translation", matrixTranslation, 3);
	if (editType == 1) ImGui::InputFloat3("Rotation", matrixRotation, 3);
	ImGui::InputFloat3("Scale", matrixScale, 3);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, objectMatrix);

	//Draw manipulation control
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(viewMatrix, projMatrix, dxshared::mCurrentGizmoOperation, dxshared::mCurrentGizmoMode, objectMatrix, NULL, NULL);

	//Set new transforms back
	objectToEdit->SetWorldMatrix4X4(DirectX::XMFLOAT4X4(objectMatrix));
#endif

	return true;
}

/* Render the objects in the scene */
void EditorScene::Render(double dt)
{
	LevelScene::Render(dt);
}