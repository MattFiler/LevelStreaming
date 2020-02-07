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

		//Populate the zone grid
		for (int i = 0; i < commands_json["CONTENT"].size(); i++) {
			json this_placement_json = commands_json["CONTENT"][i];
			ModelPlacement this_placement = ModelPlacement();
			this_placement.modelName = this_placement_json["MODEL"].get<std::string>();
			this_placement.position = DirectX::XMFLOAT3(this_placement_json["PLACEMENT"]["POSITION"][0], this_placement_json["PLACEMENT"]["POSITION"][1], this_placement_json["PLACEMENT"]["POSITION"][2]);
			this_placement.rotation = DirectX::XMFLOAT3(this_placement_json["PLACEMENT"]["ROTATION"][0], this_placement_json["PLACEMENT"]["ROTATION"][1], this_placement_json["PLACEMENT"]["ROTATION"][2]);
			this_placement.scale = DirectX::XMFLOAT3(this_placement_json["PLACEMENT"]["SCALE"][0], this_placement_json["PLACEMENT"]["SCALE"][1], this_placement_json["PLACEMENT"]["SCALE"][2]);
			LevelZoneTile* tile = level_grid->GetTileAtPosition(DirectX::XMFLOAT2(this_placement.position.x, this_placement.position.z));
			if (tile)
			{
				tile->AddModelRef(this_placement);
			}
			else
			{
				Debug::Log("ERROR: Tried to load a model " + this_placement.modelName + " to tile, but no tile covers its origin. Map editor failure!");
			}
		}

		//Load all model metadata
		for (int i = 0; i < models_json["MODELS"].size(); i++)
		{
			ModelDef this_model = ModelDef();
			this_model.modelName = models_json["MODELS"][i]["NAME"].get<std::string>();
			this_model.modelPath = level_path + models_json["MODELS"][i]["PATH"].get<std::string>();
			level_grid->AddLevelModel(this_model);
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
	ImGui::Checkbox("Lock Camera", &camLock);
#ifdef _DEBUG
	ImGui::SameLine();
	ImGui::Checkbox("Show All Zones", &dxshared::drawBoundingBoxes);
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

	//Check to see if the camera (player) has entered a zone - call to load if it has
	if (!in_editor_mode && level_grid) {
		LevelZoneTile* activeTile = level_grid->GetTileAtPosition(DirectX::XMFLOAT2(main_cam.GetPosition().x, main_cam.GetPosition().z));
		if (activeTile) {
			activeTile->LoadTile();
		}

		for (int i = 0; i < level_grid->GetAllTiles().size(); i++) {
			if (level_grid->GetAllTiles()[i] != activeTile) level_grid->GetAllTiles()[i]->UnloadTile();
		}
	}

	return true;
}

/* Render the objects in the scene */
void LevelScene::Render(double dt)
{
	GameObjectManager::Render(dt);
}