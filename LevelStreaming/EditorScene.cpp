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
	selectedEditNPC = 0;
	subdivisionCount = (level_grid) ? level_grid->subdivisionCount : 10;
	selectedNewModelIndex = 0;
	hasDoneEditorPreload = false; //?
	showModelSelector = false;
	showNpcModelSelector = false;
	showNpcEditor = false;
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
	ImGui::Text("Zone Generation Properties");
	ImGui::InputInt("Subdivision", &subdivisionCount);
	if (subdivisionCount < 1) subdivisionCount = 1;
	ImGui::Separator();

	ImGui::RadioButton("Edit NPCs", &currentEditorMode, 0); ImGui::SameLine();
	ImGui::RadioButton("Edit Triggers", &currentEditorMode, 1);
	ImGui::RadioButton("Edit Models", &currentEditorMode, 2); ImGui::SameLine();
	ImGui::RadioButton("Edit Spawnpoint", &currentEditorMode, 3);
	ImGui::Separator();
	switch (currentEditorMode) {
		case 0:
			//NPC edit controls
			ImGui::Text("Edit Level NPCs");
			ImGui::Separator();
			if (ImGui::Button("Add New NPC")) {
				selectedNewModelIndex = 0;
				showNpcModelSelector = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove Selected NPC")) {
				GameObjectManager::RemoveObject(level_grid->levelNPCs.at(selectedEditNPC));
				delete level_grid->levelNPCs.at(selectedEditNPC);
				level_grid->levelNPCs.erase(level_grid->levelNPCs.begin() + selectedEditNPC);
			}
			if (ImGui::CollapsingHeader("NPCs In Level", ImGuiTreeNodeFlags_DefaultOpen)) {
				for (int x = 0; x < level_grid->levelNPCs.size(); x++) {
					ImGui::RadioButton(("[" + std::to_string(x) + "] - " + level_grid->levelNPCs.at(x)->GetName()).c_str(), &selectedEditNPC, x);
				}
			}
			if (ImGui::Button("Edit NPC Waypoints & Speed")) {
				showNpcEditor = true;
			}
			break;
		case 1:
			//Trigger edit controls
			ImGui::Text("Edit Level Triggers");
			ImGui::Text("WIP");
			break;
			ImGui::Separator();
			if (ImGui::Button("Add New Trigger")) {
				selectedNewModelIndex = 0;
				showModelSelector = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Remove Selected Trigger")) {
				GameObjectManager::RemoveObject(level_grid->levelNPCs.at(selectedEditNPC));
				delete level_grid->levelNPCs.at(selectedEditNPC);
				level_grid->levelNPCs.erase(level_grid->levelNPCs.begin() + selectedEditNPC);
			}
			if (ImGui::CollapsingHeader("Triggers In Level", ImGuiTreeNodeFlags_DefaultOpen)) {
				for (int x = 0; x < level_grid->levelNPCs.size(); x++) {
					ImGui::RadioButton(("[" + std::to_string(x) + "] - " + level_grid->levelNPCs.at(x)->GetName()).c_str(), &selectedEditNPC, x);
				}
			}
			break;
		case 2:
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
			break;
		case 3:
			//Player spawn edit controls
			ImGui::Text("Edit Level Spawnpoint");
			ImGui::Text("WIP");
			break;
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

			//Write NPCs
			for (int i = 0; i < level_grid->levelNPCs.size(); i++) {
				commands_json_out["NPCS"][i]["MODEL"] = level_grid->levelNPCs.at(i)->GetModelName();
				commands_json_out["NPCS"][i]["PLACEMENT"]["POSITION"][0] = allActiveModels.at(i)->GetPosition().x;
				commands_json_out["NPCS"][i]["PLACEMENT"]["POSITION"][1] = allActiveModels.at(i)->GetPosition().y;
				commands_json_out["NPCS"][i]["PLACEMENT"]["POSITION"][2] = allActiveModels.at(i)->GetPosition().z;
				commands_json_out["NPCS"][i]["PLACEMENT"]["ROTATION"][0] = allActiveModels.at(i)->GetRotation(false).x;
				commands_json_out["NPCS"][i]["PLACEMENT"]["ROTATION"][1] = allActiveModels.at(i)->GetRotation(false).y;
				commands_json_out["NPCS"][i]["PLACEMENT"]["ROTATION"][2] = allActiveModels.at(i)->GetRotation(false).z;
				commands_json_out["NPCS"][i]["PLACEMENT"]["SCALE"][0] = allActiveModels.at(i)->GetScale().x;
				commands_json_out["NPCS"][i]["PLACEMENT"]["SCALE"][1] = allActiveModels.at(i)->GetScale().y;
				commands_json_out["NPCS"][i]["PLACEMENT"]["SCALE"][2] = allActiveModels.at(i)->GetScale().z;
				commands_json_out["NPCS"][i]["NPC_NAME"] = level_grid->levelNPCs.at(i)->GetName();
				for (int x = 0; x < level_grid->levelNPCs.at(i)->GetPathingPoints().size(); x++) {
					commands_json_out["NPCS"][i]["PATHING_POINTS"][x][0] = level_grid->levelNPCs.at(i)->GetPathingPoints().at(x).x;
					commands_json_out["NPCS"][i]["PATHING_POINTS"][x][1] = level_grid->levelNPCs.at(i)->GetPathingPoints().at(x).y;
					commands_json_out["NPCS"][i]["PATHING_POINTS"][x][2] = level_grid->levelNPCs.at(i)->GetPathingPoints().at(x).z;
				}
				commands_json_out["NPCS"][i]["PATHING_SPEED"] = level_grid->levelNPCs.at(i)->GetPathingSpeed();
			}

			//Write triggers
			for (int i = 0; i < level_grid->levelTriggers.size(); i++) {
				commands_json_out["TRIGGERS"][i]["BOTTOM_LEFT"][0] = level_grid->levelTriggers.at(i)->GetDims().localBottomLeft.x + level_grid->levelTriggers.at(i)->GetPosition().x;
				commands_json_out["TRIGGERS"][i]["BOTTOM_LEFT"][1] = level_grid->levelTriggers.at(i)->GetDims().localBottomLeft.y + level_grid->levelTriggers.at(i)->GetPosition().y;
				commands_json_out["TRIGGERS"][i]["BOTTOM_LEFT"][2] = level_grid->levelTriggers.at(i)->GetDims().localBottomLeft.z + level_grid->levelTriggers.at(i)->GetPosition().z;
				commands_json_out["TRIGGERS"][i]["TOP_RIGHT"][0] = level_grid->levelTriggers.at(i)->GetDims().localTopRight.x + level_grid->levelTriggers.at(i)->GetPosition().x;
				commands_json_out["TRIGGERS"][i]["TOP_RIGHT"][1] = level_grid->levelTriggers.at(i)->GetDims().localTopRight.y + level_grid->levelTriggers.at(i)->GetPosition().y;
				commands_json_out["TRIGGERS"][i]["TOP_RIGHT"][2] = level_grid->levelTriggers.at(i)->GetDims().localTopRight.z + level_grid->levelTriggers.at(i)->GetPosition().z;
			}

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

	//Allow a new model to be added to the scene from our model pool (FOR LEVEL GEO)
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

	//Allow a new model to be added to the scene from our model pool (FOR NPCS)
	if (showNpcModelSelector) {
		ImGui::Begin("NPC Creator", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::Text("NPC Model");
		ImGui::Separator();
		for (int i = 0; i < level_grid->levelModels.size(); i++) {
			ImGui::RadioButton((level_grid->levelModels.at(i).modelName).c_str(), &selectedNewModelIndex, i);
		}
		ImGui::Separator(); 
		char npcName[128] = ""; ImGui::Text("NPC Name"); ImGui::SameLine();
		if (ImGui::InputText("", npcName, IM_ARRAYSIZE(npcName), ImGuiInputTextFlags_EnterReturnsTrue)) {
			std::string newNpcName(npcName);

			NPC* new_npc = new NPC();
			new_npc->SetName(newNpcName);
			new_npc->SetData(level_grid->LoadModelToLevel(level_grid->levelModels.at(selectedNewModelIndex).modelPath_LOD1, LevelOfDetail::HIGH));
			new_npc->Create();
			new_npc->CreateModel();
			GameObjectManager::AddObject(new_npc);
			level_grid->AddNPC(new_npc);

			selectedEditNPC = level_grid->levelNPCs.size() - 1;

			showNpcModelSelector = false;
			showPopup = true;
			popupString = "Added new NPC: " + newNpcName;
		}
		ImGui::End();
	}

	//Let the user add/remove NPC waypoints, and change their speed
	if (showNpcEditor) {
		ImGui::Begin("NPC Editor", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);
		ImGui::Text("WIP");
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