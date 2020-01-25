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
		ZoneDef* this_zone = new ZoneDef();
		for (int x = 0; x < commands_json["ZONES"][i]["CONTENT"].size(); x++)
		{
			auto this_placement_json = commands_json["ZONES"][i]["CONTENT"][x];
			ModelPlacement this_placement = ModelPlacement();
			this_placement.modelName = this_placement_json["MODEL"].get<std::string>();
			this_placement.position = DirectX::XMFLOAT3(this_placement_json["PLACEMENT"]["POSITION"][0], this_placement_json["PLACEMENT"]["POSITION"][1], this_placement_json["PLACEMENT"]["POSITION"][2]);
			this_placement.rotation = DirectX::XMFLOAT3(this_placement_json["PLACEMENT"]["ROTATION"][0], this_placement_json["PLACEMENT"]["ROTATION"][1], this_placement_json["PLACEMENT"]["ROTATION"][2]);
			this_zone->models.push_back(this_placement);
		}
		this_zone->zoneBounds = new BoundingBox();
		this_zone->zoneBounds->SetDims(
			DirectX::XMFLOAT3(commands_json["ZONES"][i]["BOUNDS"][0][0], commands_json["ZONES"][i]["BOUNDS"][0][1], commands_json["ZONES"][i]["BOUNDS"][0][2]),
			DirectX::XMFLOAT3(commands_json["ZONES"][i]["BOUNDS"][1][0], commands_json["ZONES"][i]["BOUNDS"][1][1], commands_json["ZONES"][i]["BOUNDS"][1][2])
		);
		GameObjectManager::AddObject(this_zone->zoneBounds); 
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
	light_source.SetPosition(DirectX::XMFLOAT3(0.5, 0.5, 0.5));

	//Position "player"
	auto spawn_pos = commands_json["PLAYER_SPAWN"];
	main_cam.SetPosition(DirectX::XMFLOAT3(spawn_pos["POSITION"][0], spawn_pos["POSITION"][1], spawn_pos["POSITION"][2]));
	main_cam.SetRotation(DirectX::XMFLOAT3(spawn_pos["ROTATION"][0], spawn_pos["ROTATION"][1], spawn_pos["ROTATION"][2]));
}

/* Release the objects in the scene */
void LevelScene::Release()
{
	for (int i = 0; i < level_zones.size(); i++) {
		delete level_zones.at(i);
	}
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

	ImGui::End();

	//As zones are loaded on a separate thread, wait to see if the loading is done, and add the object references if so
	//We don't add the object references on the thread, as it may happen at a time between update and render, causing issues
	for (int i = 0; i < zone_load_queue.size(); i++) {
		if (!level_zones[zone_load_queue.at(i)]->isLoaded) continue;
		for (int x = 0; x < level_zones[zone_load_queue.at(i)]->loadedModels.size(); x++)
		{
			GameObjectManager::AddObject(level_zones[zone_load_queue.at(i)]->loadedModels.at(x));
		}
		should_update_queue = true;
		Debug::Log("Load of zone " + std::to_string(zone_load_queue.at(i)) + " complete!");
	}

	//If a threaded zone load operation finished, remove it from the queue
	if (should_update_queue) {
		std::vector<int> new_zone_load_queue = std::vector<int>();
		for (int i = 0; i < zone_load_queue.size(); i++) {
			if (level_zones[zone_load_queue.at(i)]->isLoaded) continue;
			new_zone_load_queue.push_back(zone_load_queue.at(i));
		}
		zone_load_queue = new_zone_load_queue;
	}

	main_cam.SetLocked(camLock);
	GameObjectManager::Update(dt);

	//Check to see if the camera (player) has entered a zone - call to load if it has
	for (int i = 0; i < level_zones.size(); i++) {
		if (level_zones.at(i)->zoneBounds->ContainsPoint(main_cam.GetPosition())) {
			if (!IsZoneLoaded(i)) {
				LoadZone(i);
			}
		}
		else
		{
			if (IsZoneLoaded(i)) {
				UnloadZone(i);
			}
		}
	}

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
	return (level_zones.at(id)->isLoading || level_zones.at(id)->isLoaded);
}

/* Load a zone by ID (threaded) */
void LevelScene::LoadZone(int id)
{
	if (IsZoneLoaded(id)) {
		Debug::Log("Requested load of zone " + std::to_string(id) + ", but it's already loaded/loading!");
		return;
	}

	Debug::Log("Starting load of zone " + std::to_string(id));
	level_zones[id]->isLoading = true;
	std::thread* load_models = new std::thread([id, this] { LoadZoneThread(id); });
	zone_load_queue.push_back(id);
}
void LevelScene::LoadZoneThread(int id)
{
	for (int i = 0; i < level_zones[id]->models.size(); i++)
	{
		for (int x = 0; x < level_models.size(); x++)
		{
			if (level_models[x].modelName == level_zones[id]->models.at(i).modelName)
			{
				Debug::Log("Loading model: " + level_zones[id]->models.at(i).modelName);
				Model* new_model = new Model();
				new_model->SetData(dxutils.LoadModel(level_path + level_models[x].modelPath));
				new_model->SetPosition(level_zones[id]->models.at(i).position);
				new_model->SetRotation(level_zones[id]->models.at(i).rotation);
				new_model->Create();
				level_zones[id]->loadedModels.push_back(new_model);
				break;
			}
		}
	}
	level_zones[id]->mutex.lock();
	level_zones[id]->isLoading = false;
	level_zones[id]->isLoaded = true;
	level_zones[id]->mutex.unlock();
}

/* Unload a zone by ID */
void LevelScene::UnloadZone(int id)
{
	if (!IsZoneLoaded(id)) {
		Debug::Log("Requested unload of zone " + std::to_string(id) + ", but it isn't loaded!");
		return;
	}

	Debug::Log("Un-loading zone " + std::to_string(id));
	for (int i = 0; i < level_zones[id]->loadedModels.size(); i++)
	{
		GameObjectManager::RemoveObject(level_zones[id]->loadedModels[i]);
	}
	level_zones[id]->loadedModels.clear();
	level_zones[id]->isLoaded = false;
}
