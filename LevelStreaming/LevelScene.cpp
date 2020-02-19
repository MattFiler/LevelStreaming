#include "LevelScene.h"

/* Load config files for level */
LevelScene::LevelScene(std::string levelName, std::string levelPath, LevelType levelType)
{
	level_name = levelName;
	level_path = levelPath;
	level_type = levelType;
}

/* Init the objects in the scene */
void LevelScene::Init()
{
	//Parse commands JSON
#if _DEBUG
	std::fstream cmd_js(level_path + "COMMANDS.JSON");
	cmd_js >> commands_json;
#else
	std::ifstream cmd_js(level_path + "COMMANDS.BIN", std::ios::in | std::ios::binary);
	std::vector<uint8_t> contents((std::istreambuf_iterator<char>(cmd_js)), std::istreambuf_iterator<char>());
	commands_json = json::from_bson(contents);
#endif

	//Parse models JSON
	std::fstream mdl_js(level_path + "MODELS_LEVEL.JSON");
	mdl_js >> models_json;

	if (level_type != LevelType::FE_LEVEL) {
		//Create the zone grid
		DirectX::XMFLOAT2 gridBottomLeft = DirectX::XMFLOAT2(commands_json["BOUNDS"]["BOTTOM_LEFT"][0], commands_json["BOUNDS"]["BOTTOM_LEFT"][1]);
		DirectX::XMFLOAT2 gridTopRight = DirectX::XMFLOAT2(commands_json["BOUNDS"]["TOP_RIGHT"][0], commands_json["BOUNDS"]["TOP_RIGHT"][1]);
		level_grid = new LevelZoneGrid(gridBottomLeft, gridTopRight, commands_json["SUBDIVISION"]);

		//Load all model metadata
		for (int i = 0; i < models_json["MODEL_DEFS"].size(); i++)
		{
			ModelDef this_model = ModelDef();
			this_model.modelName = models_json["MODEL_DEFS"][i]["NAME"].get<std::string>();
			this_model.modelPath_LOD1 = level_path + models_json["MODEL_DEFS"][i]["PATH_LOD1"].get<std::string>();
			this_model.modelPath_LOD2 = level_path + models_json["MODEL_DEFS"][i]["PATH_LOD2"].get<std::string>();
			level_grid->AddLevelModel(this_model);
		}

		//Load all NPC placements to the whole grid
		for (int i = 0; i < commands_json["NPCS"].size(); i++) {
			json this_placement_json = commands_json["NPCS"][i];
			NPC* new_npc = new NPC();
			new_npc->SetModelName(this_placement_json["MODEL"]);
			new_npc->SetPosition(DirectX::XMFLOAT3(this_placement_json["PLACEMENT"]["POSITION"][0], this_placement_json["PLACEMENT"]["POSITION"][1], this_placement_json["PLACEMENT"]["POSITION"][2]));
			new_npc->SetRotation(DirectX::XMFLOAT3(this_placement_json["PLACEMENT"]["ROTATION"][0], this_placement_json["PLACEMENT"]["ROTATION"][1], this_placement_json["PLACEMENT"]["ROTATION"][2]));
			new_npc->SetScale(DirectX::XMFLOAT3(this_placement_json["PLACEMENT"]["SCALE"][0], this_placement_json["PLACEMENT"]["SCALE"][1], this_placement_json["PLACEMENT"]["SCALE"][2]));
			new_npc->SetName(this_placement_json["NPC_NAME"]);
			for (int x = 0; x < this_placement_json["PATHING_POINTS"].size(); x++) {
				new_npc->AddPathingPoint(DirectX::XMFLOAT3(this_placement_json["PATHING_POINTS"][x][0], this_placement_json["PATHING_POINTS"][x][1], this_placement_json["PATHING_POINTS"][x][2]));
			}
			new_npc->SetPathingSpeed(this_placement_json["PATHING_SPEED"]);
			GameObjectManager::AddObject(new_npc);
			level_grid->AddNPC(new_npc);
		}
		level_grid->ForceLoadNPCS();

		//Load all trigger placements to the whole grid
		for (int i = 0; i < commands_json["TRIGGERS"].size(); i++) {
			json this_placement_json = commands_json["TRIGGERS"][i];
			Trigger* new_trigger = new Trigger();
			new_trigger->SetDims(
				DirectX::XMFLOAT3(this_placement_json["BOTTOM_LEFT"][0], this_placement_json["BOTTOM_LEFT"][1], this_placement_json["BOTTOM_LEFT"][2]),
				DirectX::XMFLOAT3(this_placement_json["TOP_RIGHT"][0], this_placement_json["TOP_RIGHT"][1], this_placement_json["TOP_RIGHT"][2])
			);
			GameObjectManager::AddObject(new_trigger);
			level_grid->AddTrigger(new_trigger);
		}

		//Load all model placements to tiles
		for (int i = 0; i < commands_json["MODELS"].size(); i++) {
			json this_placement_json = commands_json["MODELS"][i];
			ModelPlacement this_placement = ModelPlacement();
			this_placement.modelName = this_placement_json["MODEL"].get<std::string>();
			this_placement.position = DirectX::XMFLOAT3(this_placement_json["PLACEMENT"]["POSITION"][0], this_placement_json["PLACEMENT"]["POSITION"][1], this_placement_json["PLACEMENT"]["POSITION"][2]);
			this_placement.rotation = DirectX::XMFLOAT3(this_placement_json["PLACEMENT"]["ROTATION"][0], this_placement_json["PLACEMENT"]["ROTATION"][1], this_placement_json["PLACEMENT"]["ROTATION"][2]);
			this_placement.scale = DirectX::XMFLOAT3(this_placement_json["PLACEMENT"]["SCALE"][0], this_placement_json["PLACEMENT"]["SCALE"][1], this_placement_json["PLACEMENT"]["SCALE"][2]);
			LevelZoneTile* tile = level_grid->GetTileAtPosition(DirectX::XMFLOAT2(this_placement.position.x, this_placement.position.z));
			if (tile) tile->AddModelRef(this_placement);
			else Debug::Log("ERROR: Tried to load a model " + this_placement.modelName + " to tile, but no tile covers its origin. Map editor failure!");
		}
	}

	//Setup cam & light source
	light_source = Light();
	main_cam = Camera();
	GameObjectManager::AddObject(&light_source);
	GameObjectManager::AddObject(&main_cam);
	GameObjectManager::Create();
	main_cam.SetLocked(level_type == LevelType::FE_LEVEL);
	light_source.SetPosition(DirectX::XMFLOAT3(0.5, 0.5, 0.5));

	//Position "player"
	auto spawn_pos = commands_json["PLAYER_SPAWN"];
	main_cam.SetPosition(DirectX::XMFLOAT3(spawn_pos["POSITION"][0], spawn_pos["POSITION"][1], spawn_pos["POSITION"][2]));
	main_cam.SetRotation(DirectX::XMFLOAT3(spawn_pos["ROTATION"][0], spawn_pos["ROTATION"][1], spawn_pos["ROTATION"][2]));
}

/* Release the objects in the scene */
void LevelScene::Release()
{
	if (level_grid) delete level_grid;
	GameObjectManager::Release();
}

/* Update the objects in the scene */
bool LevelScene::Update(double dt)
{
	bool open = true;
	bool camLock = main_cam.GetLocked();

	ImGui::SetNextWindowPos(ImVec2(950, 0));
	ImGui::SetNextWindowSize(ImVec2(330, 720));
	ImGui::Begin("Controls", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

	ImGui::Text("Scene Controls");
	ImGui::Separator();
	ImGui::Checkbox("Lock Cam", &camLock);
#ifdef _DEBUG
	ImGui::SameLine();
	ImGui::Checkbox("Show Triggers", &dxshared::drawBoundingBoxes);
	ImGui::SameLine();
	ImGui::Checkbox("Stop NPCs", &dxshared::pauseNPCs);
#endif

	ImGui::Separator();
	ImGui::Text(("Level: " + level_path).c_str());

	ImGui::Separator();
	ImGui::Text(("Cam Pos X: " + std::to_string(main_cam.GetPosition().x) + ", Y: " + std::to_string(main_cam.GetPosition().y) + "Z: " + std::to_string(main_cam.GetPosition().z)).c_str());
	ImGui::Text(("Cam Rot X: " + std::to_string(main_cam.GetRotation().x) + ", Y: " + std::to_string(main_cam.GetRotation().y) + "Z: " + std::to_string(main_cam.GetRotation().z)).c_str());

	ImGui::Separator();
	ImGui::SliderFloat("Ambient R", &dxshared::ambientLightColour.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient G", &dxshared::ambientLightColour.y, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient B", &dxshared::ambientLightColour.z, 0.0f, 1.0f);

	ImGui::End();

	if (level_grid) level_grid->TrackLoading();

	main_cam.SetLocked(camLock);
	GameObjectManager::Update(dt);

	if (!in_editor_mode && level_grid) {
		//Get the tile the camera (player) is currently within
		LevelZoneTile* activeTile = level_grid->GetTileAtPosition(DirectX::XMFLOAT2(main_cam.GetPosition().x, main_cam.GetPosition().z));
		std::vector<LevelZoneTile*> loadedTiles = std::vector<LevelZoneTile*>();
		if (activeTile) {
			//Load the tile we're within
			activeTile->LoadTile(LevelOfDetail::HIGH);
			loadedTiles.push_back(activeTile);

			//Get neighbours to the tile we're within, and load them at a lower LOD
			std::vector<LevelZoneTile*> neighbourTiles = level_grid->GetTileNeighbours(activeTile).AllNeighbours;
			for (int i = 0; i < neighbourTiles.size(); i++) {
				neighbourTiles[i]->LoadTile(LevelOfDetail::LOW);
				loadedTiles.push_back(neighbourTiles[i]);
			}
		}

		//Unload all inactive tiles
		for (int i = 0; i < level_grid->GetAllTiles().size(); i++) {
			bool shouldUnload = true;
			for (int x = 0; x < loadedTiles.size(); x++) {
				if (level_grid->GetAllTiles()[i] == loadedTiles[x]) shouldUnload = false;
			}
			if (!shouldUnload) continue; //Only unload old LODs
			level_grid->GetAllTiles()[i]->UnloadTile(); //Unload everything if loaded
		}
	}

	return true;
}

/* Render the objects in the scene */
void LevelScene::Render(double dt)
{
	GameObjectManager::Render(dt);
}