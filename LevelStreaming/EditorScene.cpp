#include "EditorScene.h"
#include <iomanip>

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
	selectedEditZone = 0;
	selectedEditModel = 0;
	editType = 0;
	selectedNewModelIndex = 0;
	showModelSelector = false;
	showPopup = false;
	dxshared::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

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
	if (!hasDoneEditorPreload && zone_load_queue.size() == 0 && allActiveModels.size() == 0) {
		for (int i = 0; i < level_zones.size(); i++) {
			allActiveZoneDummys.push_back(level_zones.at(i)->zoneBounds);
			for (int x = 0; x < level_zones.at(i)->loadedModels.size(); x++) {
				allActiveModelNames.push_back(level_zones.at(i)->models.at(x).modelName);
				allActiveModels.push_back(level_zones.at(i)->loadedModels.at(x));
			}
		}
		hasDoneEditorPreload = true;
	}

	ImGui::SetNextWindowPos(ImVec2(950, 200));
	ImGui::SetNextWindowSize(ImVec2(330, 390));
	ImGui::Begin("Level Editor", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
	ImGui::Text("Level Editor");
	ImGui::Separator();

	//Zone edit controls
	ImGui::RadioButton("Edit Zone Transforms", &editType, 0);
	ImGui::Separator();
	if (ImGui::Button("Add New Zone")) {
		BoundingBox* zoneDummy = new BoundingBox();
		zoneDummy->Create();
		GameObjectManager::AddObject(zoneDummy);
		allActiveZoneDummys.push_back(zoneDummy);

		editType = 0;
		selectedEditZone = allActiveZoneDummys.size() - 1;

		showPopup = true;
		popupString = "Added new zone!";
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove Selected Zone")) {
		GameObjectManager::RemoveObject(allActiveZoneDummys.at(selectedEditZone));
		delete allActiveZoneDummys.at(selectedEditZone);
		allActiveZoneDummys.erase(allActiveZoneDummys.begin() + selectedEditZone);
	}
	if (ImGui::CollapsingHeader("Zones In Level", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (int x = 0; x < allActiveZoneDummys.size(); x++) {
			ImGui::RadioButton(("Zone " + std::to_string(x)).c_str(), &selectedEditZone, x);
		}
	}
	ImGui::Separator();

	//Model edit controls
	ImGui::RadioButton("Edit Model Transforms", &editType, 1);
	ImGui::Separator();
	if (ImGui::Button("Add New Model")) {
		selectedNewModelIndex = 0;
		showModelSelector = true;
	}
	ImGui::SameLine();
	if (ImGui::Button("Remove Selected Model")) {
		GameObjectManager::RemoveObject(allActiveModels.at(selectedEditModel));
		delete allActiveModels.at(selectedEditModel);
		allActiveModels.erase(allActiveModels.begin() + selectedEditModel);
		allActiveModelNames.erase(allActiveModelNames.begin() + selectedEditModel);
	}
	if (ImGui::CollapsingHeader("Models In Level", ImGuiTreeNodeFlags_DefaultOpen)) {
		for (int x = 0; x < allActiveModels.size(); x++) {
			ImGui::RadioButton(("[" + std::to_string(x) + "] - " + allActiveModelNames.at(x)).c_str(), &selectedEditModel, x);
		}
	}
	ImGui::Separator();

	//Save level
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
					commands_json_out["ZONES"][i]["CONTENT"][count]["PLACEMENT"]["ROTATION"][0] = allActiveModels.at(y)->GetRotation(false).x;
					commands_json_out["ZONES"][i]["CONTENT"][count]["PLACEMENT"]["ROTATION"][1] = allActiveModels.at(y)->GetRotation(false).y;
					commands_json_out["ZONES"][i]["CONTENT"][count]["PLACEMENT"]["ROTATION"][2] = allActiveModels.at(y)->GetRotation(false).z;
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
			showPopup = true;
			popupString = "Failed to save " + std::to_string(allActiveModels.size() - saved_models.size()) + " models! Check zone placement.";
			Debug::Log(popupString);
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

			std::ofstream commands_json_file2(level_path + "COMMANDS.JSON");
			commands_json_file2 << std::setw(4) << commands_json_out << std::endl;
			commands_json_file2.close();

			showPopup = true;
			popupString = "Saved level!";
			Debug::Log(popupString);
		}
	}
	ImGui::End();

	//Show a popup
	if (showPopup) {
		ImGui::Begin("Popup", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::Text(popupString.c_str());
		if (ImGui::Button("OK")) {
			showPopup = false;
		}
		ImGui::End();
	}

	//Allow a new model to be added to the scene from our model pool
	if (showModelSelector) {
		ImGui::Begin("Available Models", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
		for (int i = 0; i < level_models.size(); i++) {
			ImGui::RadioButton((level_models.at(i).modelName).c_str(), &selectedNewModelIndex, i);
		}
		if (ImGui::Button("Add Selected Model")) {
			Model* new_model = new Model();
			new_model->SetData(LoadModelToLevel(level_path + level_models.at(selectedNewModelIndex).modelPath));
			new_model->Create();
			GameObjectManager::AddObject(new_model);
			allActiveModelNames.push_back(level_models.at(selectedNewModelIndex).modelName);
			allActiveModels.push_back(new_model);

			editType = 1;
			selectedEditModel = allActiveModels.size() - 1;

			showModelSelector = false;
			showPopup = true;
			popupString = "Added new model: " + level_models.at(selectedNewModelIndex).modelName;
		}
		ImGui::End();
	}

	//Only continue if our requested edit object is valid
	GameObject* objectToEdit = nullptr;
	if (editType == 1)
	{
		if (allActiveModels.size() <= selectedEditModel) return true;
		objectToEdit = allActiveModels.at(selectedEditModel);
	}
	else
	{
		if (dxshared::mCurrentGizmoOperation == ImGuizmo::ROTATE) dxshared::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (allActiveZoneDummys.size() <= selectedEditZone) return true;
		objectToEdit = allActiveZoneDummys.at(selectedEditZone);
	}

	//Show active edit zone
	for (int i = 0; i < allActiveZoneDummys.size(); i++) {
		allActiveZoneDummys.at(i)->ShowVisual(i == selectedEditZone && editType == 0);
	}

	//Get matrices as float arrays
	float* objectMatrix = &objectToEdit->GetWorldMatrix4X4().m[0][0];
	float* projMatrix = &dxutils.MatrixToFloat4x4(dxshared::mProjection).m[0][0];
	float* viewMatrix = &dxutils.MatrixToFloat4x4(dxshared::mView).m[0][0];

	//Show options to swap between different transforms
	ImGui::SetNextWindowPos(ImVec2(950, 590));
	ImGui::SetNextWindowSize(ImVec2(330, 130));
	ImGui::Begin("Transform Controls", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
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

	//Draw manipulation control
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(viewMatrix, projMatrix, dxshared::mCurrentGizmoOperation, dxshared::mCurrentGizmoMode, objectMatrix, NULL, NULL); 

	//Get values from manipulation
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(objectMatrix, matrixTranslation, matrixRotation, matrixScale);

	//We don't allow gizmo editing of rotation, as ImGuizmo's accuracy really sucks, and throws everything off
	matrixRotation[0] = objectToEdit->GetRotation(false).x;
	matrixRotation[1] = objectToEdit->GetRotation(false).y;
	matrixRotation[2] = objectToEdit->GetRotation(false).z;

	//Allow text overwrite
	ImGui::InputFloat3("Translation", matrixTranslation, 3);
	if (editType == 1) ImGui::InputFloat3("Rotation", matrixRotation, 3);
	ImGui::InputFloat3("Scale", matrixScale, 3);

	//Set new transforms back
	objectToEdit->SetPosition(DirectX::XMFLOAT3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]));
	objectToEdit->SetRotation(DirectX::XMFLOAT3(matrixRotation[0], matrixRotation[1], matrixRotation[2]));
	objectToEdit->SetScale(DirectX::XMFLOAT3(matrixScale[0], matrixScale[1], matrixScale[2]));
	ImGui::End();
#endif

	return true;
}

/* Render the objects in the scene */
void EditorScene::Render(double dt)
{
	LevelScene::Render(dt);
}