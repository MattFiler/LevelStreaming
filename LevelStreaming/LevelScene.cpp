#include "LevelScene.h"

/* Init the objects in the scene */
void LevelScene::Init()
{
	//Parse commands JSON
	std::fstream cmd_js("DATA\\ENV\\PRODUCTION\\TEST_LEVEL\\COMMANDS.JSON");
	cmd_js >> commands_json;

	//Parse models JSON
	std::fstream mdl_js("DATA\\ENV\\PRODUCTION\\TEST_LEVEL\\MODELS_LEVEL.JSON");
	mdl_js >> models_json;

	//Load all zone metadata
	for (int i = 0; i < commands_json["ZONES"].size(); i++) {
		ZoneDef this_zone = ZoneDef();
		for (int x = 0; x < commands_json["ZONES"][i].size(); x++) {
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
	for (int i = 0; i < models_json["MODELS"].size(); i++) {
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
	main_cam.SetLocked(false);

	//Position "player"
	auto spawn_pos = commands_json["PLAYER_SPAWN"];
	main_cam.SetPosition(DirectX::XMFLOAT3(spawn_pos["POSITION"][0], spawn_pos["POSITION"][1], spawn_pos["POSITION"][2]));
	main_cam.SetRotation(DirectX::XMFLOAT3(spawn_pos["ROTATION"][0], spawn_pos["ROTATION"][1], spawn_pos["ROTATION"][2]));
}

/* Release the objects in the scene */
void LevelScene::Release()
{
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
	ImGui::Text(("Cam Pos X: " + std::to_string(main_cam.GetPosition().x) + ", Y: " + std::to_string(main_cam.GetPosition().y) + "Z: " + std::to_string(main_cam.GetPosition().z)).c_str());
	ImGui::Text(("Cam Rot X: " + std::to_string(main_cam.GetRotation().x) + ", Y: " + std::to_string(main_cam.GetRotation().y) + "Z: " + std::to_string(main_cam.GetRotation().z)).c_str());

	ImGui::Separator();
	ImGui::SliderFloat("Ambient R", &dxshared::ambientLightColour.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient G", &dxshared::ambientLightColour.y, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient B", &dxshared::ambientLightColour.z, 0.0f, 1.0f);

	ImGui::Separator();
	if (ImGui::Button("Load Zone 1"))
	{
		LoadZone(0);
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Zone 2"))
	{
		LoadZone(1);
	}

	ImGui::Separator();
	if (ImGui::Button("Unload Zone 1"))
	{
		UnloadZone(0);
	}
	ImGui::SameLine();
	if (ImGui::Button("Unload Zone 2"))
	{
		UnloadZone(1);
	}

	ImGui::End();

	main_cam.SetLocked(camLock);
	GameObjectManager::Update(dt);

	return true;
}

/* Render the objects in the scene */
void LevelScene::Render(double dt)
{
	GameObjectManager::Render(dt);
}

/* Load a zone by ID */
void LevelScene::LoadZone(int id)
{
	Debug::Log("Loading zone " + id);
	ZoneDef* this_zone = &level_zones[id];
	for (int i = 0; i < this_zone->models.size(); i++)
	{
		for (int x = 0; x < level_models.size(); x++)
		{
			if (level_models[x].modelName == this_zone->models.at(i).modelName)
			{
				Debug::Log("Loading model: " + this_zone->models.at(i).modelName);
				Model* new_model = new Model();
				new_model->SetData(dxutils.LoadModel("DATA\\ENV\\PRODUCTION\\TEST_LEVEL\\" + level_models[x].modelPath));
				new_model->SetPosition(this_zone->models.at(i).position);
				new_model->SetRotation(this_zone->models.at(i).rotation);
				new_model->Create();
				GameObjectManager::AddObject(new_model);
				this_zone->loadedModels.push_back(new_model);
				break;
			}
		}
	}
}

/* Unload a zone by ID */
void LevelScene::UnloadZone(int id)
{
	Debug::Log("Un-loading zone " + id);
	ZoneDef* this_zone = &level_zones[id];
	for (int i = 0; i < this_zone->loadedModels.size(); i++)
	{
		GameObjectManager::RemoveObject(this_zone->loadedModels[i]);
	}
}
