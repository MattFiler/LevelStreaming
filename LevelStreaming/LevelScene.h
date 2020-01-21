#pragma once

#include "Scene.h"

class LevelScene : public Scene
{
public:
	void Init() override;
	void Release() override;

	bool Update(double dt) override;
	void Render(double dt) override;

	void LoadZone(int id);
	void UnloadZone(int id);

private:
	Utilities dxutils = Utilities();

	json commands_json;
	json models_json;

	Camera main_cam;
	Light light_source;

	//Parsed from json
	std::vector<ZoneDef> level_zones = std::vector<ZoneDef>();
	std::vector<ModelDef> level_models = std::vector<ModelDef>();
};

