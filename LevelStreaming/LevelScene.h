#pragma once

#include "Scene.h"

class LevelScene : public Scene
{
public:
	LevelScene(std::string levelName, std::string levelPath, LevelType levelType);

	void Init() override;
	void Release() override;

	bool Update(double dt) override;
	void Render(double dt) override;

	std::string GetName() {
		return level_name;
	}

protected:
	bool IsZoneLoaded(int id);
	void LoadZone(int id);
	void LoadZoneThread(int id);
	void UnloadZone(int id);

	Utilities dxutils = Utilities();

	std::string level_name;
	std::string level_path;
	LevelType level_type;

	json commands_json;
	json models_json;

	bool in_editor_mode = false;

	Camera main_cam;
	Light light_source;

	std::vector<ZoneDef*> level_zones = std::vector<ZoneDef*>();
	std::vector<ModelDef> level_models = std::vector<ModelDef>();

	bool should_update_queue = false;
	std::vector<int> zone_load_queue = std::vector<int>();
};

