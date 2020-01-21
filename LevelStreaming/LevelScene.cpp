#include "LevelScene.h"

/* Load config files for level */
LevelScene::LevelScene(std::string levelName, std::string levelPath, LevelType levelType)
{
	level_name = levelName;
	level_path = levelPath;
	level_type = levelType;

	//Parse commands JSON
	std::fstream cmd_js(level_path + "COMMANDS.JSON");
	cmd_js >> commands_json;

	//Parse models JSON
	std::fstream mdl_js(level_path + "MODELS_LEVEL.JSON");
	mdl_js >> models_json;
}

/* Init the objects in the scene */
void LevelScene::Init()
{
	//Load all zone metadata
	for (int i = 0; i < commands_json["ZONES"].size(); i++) 
	{
		ZoneDef this_zone = ZoneDef();
		for (int x = 0; x < commands_json["ZONES"][i].size(); x++)
		{
			auto this_placement_json = commands_json["ZONES"][i][x];
			ModelPlacement this_placement = ModelPlacement();
			this_placement.modelName = this_placement_json["MODEL"].get<std::string>();
			this_placement.position = DirectX::XMFLOAT3(this_placement_json["PLACEMENT"]["POSITION"][0], this_placement_json["PLACEMENT"]["POSITION"][1], this_placement_json["PLACEMENT"]["POSITION"][2]);
			this_placement.rotation = DirectX::XMFLOAT3(this_placement_json["PLACEMENT"]["ROTATION"][0], this_placement_json["PLACEMENT"]["ROTATION"][1], this_placement_json["PLACEMENT"]["ROTATION"][2]);
			this_zone.models.push_back(this_placement);
		}
		level_zones.push_back(this_zone);
	}

	//Load all model metadata
	for (int i = 0; i < models_json["MODELS"].size(); i++) 
	{
		ModelDef this_model = ModelDef();
		this_model.modelName = models_json["MODELS"][i]["NAME"].get<std::string>();
		this_model.modelPath = models_json["MODELS"][i]["PATH"].get<std::string>();
		level_models.push_back(this_model);
	}

	//Setup cam & light source
	light_source = Light();
	main_cam = Camera();
	GameObjectManager::AddObject(&light_source);
	GameObjectManager::AddObject(&main_cam);
	GameObjectManager::Create();
	main_cam.SetLocked(level_type == LevelType::FE_LEVEL);

	//Position "player"
	auto spawn_pos = commands_json["PLAYER_SPAWN"];
	main_cam.SetPosition(DirectX::XMFLOAT3(spawn_pos["POSITION"][0], spawn_pos["POSITION"][1], spawn_pos["POSITION"][2]));
	main_cam.SetRotation(DirectX::XMFLOAT3(spawn_pos["ROTATION"][0], spawn_pos["ROTATION"][1], spawn_pos["ROTATION"][2]));
}

/* Release the objects in the scene */
void LevelScene::Release()
{
	level_zones.clear();
	level_models.clear();
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
	ImGui::Checkbox("Enable Debugging", &dxshared::enableDebug);
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

	ImGui::Separator();
	for (int i = 0; i < level_zones.size(); i++)
	{
		if (ImGui::Button(("Load Zone " + std::to_string(i + 1)).c_str()))
		{
			LoadZone(i);
		}
	}

	ImGui::Separator();
	for (int i = 0; i < level_zones.size(); i++) 
	{
		if (ImGui::Button(("Unload Zone " + std::to_string(i + 1)).c_str()))
		{
			UnloadZone(i);
		}
	}

	ImGui::End();

	//As zones are loaded on a separate thread, wait to see if the loading is done, and add the object references if so
	//We don't add the object references on the thread, as it may happen at a time between update and render, causing issues
	for (int i = 0; i < zone_load_queue.size(); i++) {
		ZoneLoadQueue* this_thread = &zone_load_queue.at(i);
		if (this_thread->completed || this_thread->executing) continue;
		for (int i = 0; i < level_zones.at(this_thread->zoneID).loadedModels.size(); i++)
		{
			GameObjectManager::AddObject(level_zones.at(this_thread->zoneID).loadedModels.at(i));
		}
		this_thread->threadOperation->join();
		this_thread->completed = true;
		should_update_queue = true;
		Debug::Log("Load of zone " + std::to_string(this_thread->zoneID) + " complete!");
	}

	//If a thread finished, remove it from the queue
	if (should_update_queue) {
		std::vector<ZoneLoadQueue> new_zone_load_queue = std::vector<ZoneLoadQueue>();
		for (int i = 0; i < zone_load_queue.size(); i++) {
			if (zone_load_queue.at(i).completed) continue;
			new_zone_load_queue.push_back(zone_load_queue.at(i));
		}
		zone_load_queue = new_zone_load_queue;
	}

	main_cam.SetLocked(camLock);
	GameObjectManager::Update(dt);

	return true;
}

/* Render the objects in the scene */
void LevelScene::Render(double dt)
{
	GameObjectManager::Render(dt);
}

/* Is the zone ID loaded? */
bool LevelScene::IsZoneLoaded(int id)
{
	return (level_zones.at(id).loadedModels.size() != 0);
}

/* Load a zone by ID (threaded) */
void LevelScene::LoadZone(int id)
{
	if (IsZoneLoaded(id)) {
		Debug::Log("Requested load of zone " + std::to_string(id) + ", but it's already loaded!");
		return;
	}

	Debug::Log("Starting load of zone " + std::to_string(id));
	std::thread* load_models = new std::thread([id, this] { LoadZoneThread(id); });
	ZoneLoadQueue this_load_thread = ZoneLoadQueue(load_models, id);
	zone_load_queue.push_back(this_load_thread);
}
void LevelScene::LoadZoneThread(int id)
{
	ZoneDef* this_zone = &level_zones[id];
	for (int i = 0; i < this_zone->models.size(); i++)
	{
		for (int x = 0; x < level_models.size(); x++)
		{
			if (level_models[x].modelName == this_zone->models.at(i).modelName)
			{
				Debug::Log("Loading model: " + this_zone->models.at(i).modelName);
				Model* new_model = new Model();
				new_model->SetData(dxutils.LoadModel(level_path + level_models[x].modelPath));
				new_model->SetPosition(this_zone->models.at(i).position);
				new_model->SetRotation(this_zone->models.at(i).rotation);
				new_model->Create();
				this_zone->loadedModels.push_back(new_model);
				break;
			}
		}
	}

	//Is this thread safe??
	for (int i = 0; i < zone_load_queue.size(); i++) {
		if (zone_load_queue.at(i).zoneID == id) {
			zone_load_queue.at(i).executing = false;
			break;
		}
	}
}

/* Unload a zone by ID */
void LevelScene::UnloadZone(int id)
{
	if (!IsZoneLoaded(id)) {
		Debug::Log("Requested unload of zone " + std::to_string(id) + ", but it isn't loaded!");
		return;
	}

	ZoneDef* this_zone = &level_zones[id];
	Debug::Log("Un-loading zone " + std::to_string(id));
	for (int i = 0; i < this_zone->loadedModels.size(); i++)
	{
		GameObjectManager::RemoveObject(this_zone->loadedModels[i]);
	}
	this_zone->loadedModels.clear();
}
