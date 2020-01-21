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

private:
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

	Camera main_cam;
	Light light_source;

	std::vector<ZoneDef> level_zones = std::vector<ZoneDef>();
	std::vector<ModelDef> level_models = std::vector<ModelDef>();

	bool should_update_queue = false;
	std::vector<ZoneLoadQueue> zone_load_queue = std::vector<ZoneLoadQueue>();
};

