#pragma once

#include "Scene.h"

class LevelScene : public Scene
{
public:
	LevelScene(std::string levelPath, bool frontendLevel);

	void Init() override;
	void Release() override;

	bool Update(double dt) override;
	void Render(double dt) override;

	void LoadZone(int id);
	void UnloadZone(int id);

private:
	Utilities dxutils = Utilities();

	std::string level_path = "";
	bool is_frontend = false;

	json commands_json;
	json models_json;

	Camera main_cam;
	Light light_source;

	std::vector<ZoneDef> level_zones = std::vector<ZoneDef>();
	std::vector<ModelDef> level_models = std::vector<ModelDef>();
};

