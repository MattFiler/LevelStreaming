#pragma once

#include "Scene.h"
#include "LevelZoneGrid.h"
#include "LevelZoneTile.h"

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
	Utilities dxutils = Utilities();

	std::string level_name;
	std::string level_path;
	LevelType level_type;

	json commands_json;
	json models_json;

	bool in_editor_mode = false;

	Camera main_cam;
	Light light_source;

	LevelZoneGrid* level_grid = nullptr;
};

