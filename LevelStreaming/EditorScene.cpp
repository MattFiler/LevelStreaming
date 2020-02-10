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
	selectedEditModel = 0;
	subdivisionCount = (level_grid) ? level_grid->subdivisionCount : 10;
	selectedNewModelIndex = 0;
	showModelSelector = false;
	showPopup = false;
	dxshared::mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

	if (level_type != LevelType::FE_LEVEL) {
		//Force-load all zones at all times in editor
		allActiveModels.clear();
		allActiveModelNames.clear();
		for (int i = 0; i < level_grid->GetAllTiles().size(); i++) {
			level_grid->GetAllTiles()[i]->LoadTile(LevelOfDetail::HIGH);
		}
	}
#endif
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
	if (level_type == LevelType::FE_LEVEL) return true;

#if _DEBUG
	//Grab references to all loaded models once loading is complete
	if (!hasDoneEditorPreload) {
		bool loadCompleted = true;
		for (int i = 0; i < level_grid->GetAllTiles().size(); i++) {
			if (!level_grid->GetAllTiles()[i]->isLoaded) {
				loadCompleted = false;
				break;
			}
		}
		if (loadCompleted) {
			for (int i = 0; i < level_grid->levelTiles.size(); i++) {
				for (int x = 0; x < level_grid->levelTiles[i]->loadedModels.size(); x++) {
					allActiveModelNames.push_back(level_grid->levelTiles[i]->models.at(x).modelName);
					allActiveModels.push_back(level_grid->levelTiles[i]->loadedModels.at(x));
				}
			}
			hasDoneEditorPreload = true;
		}
	}

	ImGui::SetNextWindowPos(ImVec2(950, 200));
	ImGui::SetNextWindowSize(ImVec2(330, 390));
	ImGui::Begin("Level Editor", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
	ImGui::Text("Level Editor");
	ImGui::Separator();

	//Zone edit controls
	ImGui::Text("Zones Generation Properties");
	ImGui::InputInt("Subdivision", &subdivisionCount);
	if (subdivisionCount < 1) subdivisionCount = 1;
	ImGui::Separator();

	//Model edit controls
	ImGui::Text("Edit Level Models");
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

		//Work out total grid bounds
		DirectX::XMFLOAT2 bottomLeft = DirectX::XMFLOAT2(0, 0);
		DirectX::XMFLOAT2 topRight = DirectX::XMFLOAT2(0, 0);
		for (int i = 0; i < allActiveModels.size(); i++) {
			DirectX::XMFLOAT3 thisPos = allActiveModels.at(i)->GetPosition();
			if (thisPos.x < bottomLeft.x) bottomLeft.x = thisPos.x - 1;
			if (thisPos.z < bottomLeft.y) bottomLeft.y = thisPos.z - 1;
			if (thisPos.x > topRight.x) topRight.x = thisPos.x + 1;
			if (thisPos.z > topRight.y) topRight.y = thisPos.z + 1;
		}

		//Make sure all models fall into the grid
		level_grid->Resize(bottomLeft, topRight, subdivisionCount);
		int failureCount = 0;
		for (int i = 0; i < allActiveModels.size(); i++) {
			LevelZoneTile* tile = level_grid->GetTileAtPosition(DirectX::XMFLOAT2(allActiveModels[i]->GetPosition().x, allActiveModels[i]->GetPosition().z));
			if (!tile) failureCount++;
		}
		if (failureCount != 0) 
		{
			//Not all models fall into grid, something wrong with the logic above that calculates bounds...
			showPopup = true;
			popupString = "Failed to save " + std::to_string(failureCount) + " models! Map editor logic error.";
			Debug::Log(popupString);
		}
		else
		{
			//Write new grid info
			commands_json_out["BOUNDS"]["BOTTOM_LEFT"][0] = bottomLeft.x;
			commands_json_out["BOUNDS"]["BOTTOM_LEFT"][1] = bottomLeft.y;
			commands_json_out["BOUNDS"]["TOP_RIGHT"][0] = topRight.x;
			commands_json_out["BOUNDS"]["TOP_RIGHT"][1] = topRight.y;
			commands_json_out["SUBDIVISION"] = subdivisionCount;

			//Write models
			for (int i = 0; i < allActiveModels.size(); i++) {
				commands_json_out["MODELS"][i]["MODEL"] = allActiveModelNames.at(i);
				commands_json_out["MODELS"][i]["PLACEMENT"]["POSITION"][0] = allActiveModels.at(i)->GetPosition().x;
				commands_json_out["MODELS"][i]["PLACEMENT"]["POSITION"][1] = allActiveModels.at(i)->GetPosition().y;
				commands_json_out["MODELS"][i]["PLACEMENT"]["POSITION"][2] = allActiveModels.at(i)->GetPosition().z;
				commands_json_out["MODELS"][i]["PLACEMENT"]["ROTATION"][0] = allActiveModels.at(i)->GetRotation(false).x;
				commands_json_out["MODELS"][i]["PLACEMENT"]["ROTATION"][1] = allActiveModels.at(i)->GetRotation(false).y;
				commands_json_out["MODELS"][i]["PLACEMENT"]["ROTATION"][2] = allActiveModels.at(i)->GetRotation(false).z;
				commands_json_out["MODELS"][i]["PLACEMENT"]["SCALE"][0] = allActiveModels.at(i)->GetScale().x;
				commands_json_out["MODELS"][i]["PLACEMENT"]["SCALE"][1] = allActiveModels.at(i)->GetScale().y;
				commands_json_out["MODELS"][i]["PLACEMENT"]["SCALE"][2] = allActiveModels.at(i)->GetScale().z;
			}

			//Write player spawn (todo: a player spawn position widget)
			commands_json_out["PLAYER_SPAWN"]["POSITION"][0] = 0;
			commands_json_out["PLAYER_SPAWN"]["POSITION"][1] = 0;
			commands_json_out["PLAYER_SPAWN"]["POSITION"][2] = 0;
			commands_json_out["PLAYER_SPAWN"]["ROTATION"][0] = 0;
			commands_json_out["PLAYER_SPAWN"]["ROTATION"][1] = 0;
			commands_json_out["PLAYER_SPAWN"]["ROTATION"][2] = 0;

			//Write out to BSON for production
			std::vector<uint8_t> bson = json::to_bson(commands_json_out);
			std::ofstream commands_json_file(level_path + "COMMANDS.BIN", std::ios::out | std::ios::binary);
			commands_json_file.write((char*)&bson[0], bson.size() * sizeof(uint8_t));
			commands_json_file.close();

			//Write out to JSON for debugging
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
		for (int i = 0; i < level_grid->levelModels.size(); i++) {
			ImGui::RadioButton((level_grid->levelModels.at(i).modelName).c_str(), &selectedNewModelIndex, i);
		}
		if (ImGui::Button("Add Selected Model")) {
			Model* new_model = new Model();
			new_model->SetData(level_grid->LoadModelToLevel(level_grid->levelModels.at(selectedNewModelIndex).modelPath_LOD1, LevelOfDetail::HIGH));
			new_model->Create();
			GameObjectManager::AddObject(new_model);
			allActiveModelNames.push_back(level_grid->levelModels.at(selectedNewModelIndex).modelName);
			allActiveModels.push_back(new_model);

			selectedEditModel = allActiveModels.size() - 1;

			showModelSelector = false;
			showPopup = true;
			popupString = "Added new model: " + level_grid->levelModels.at(selectedNewModelIndex).modelName;
		}
		ImGui::End();
	}

	//Check our model buffers again (tacked on for editor)
	if (level_grid) level_grid->TrackLoading();

	//Only continue if our requested edit object is valid
	if (allActiveModels.size() <= selectedEditModel) return true;
	GameObject* objectToEdit = allActiveModels.at(selectedEditModel);

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

	//Get values from manipulation
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(objectMatrix, matrixTranslation, matrixRotation, matrixScale);

	//We don't allow gizmo editing of rotation, as ImGuizmo's accuracy really sucks, and throws everything off
	matrixRotation[0] = objectToEdit->GetRotation(false).x;
	matrixRotation[1] = objectToEdit->GetRotation(false).y;
	matrixRotation[2] = objectToEdit->GetRotation(false).z;

	//Allow text overwrite
	ImGui::InputFloat3("Translation", matrixTranslation, 3);
	ImGui::InputFloat3("Rotation", matrixRotation, 3);
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