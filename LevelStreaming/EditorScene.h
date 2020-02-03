#pragma once

#include "LevelScene.h"

class EditorScene : public LevelScene
{
public:
	EditorScene(std::string levelName, std::string levelPath, LevelType levelType);

	void Init() override;
	void Release() override;

	bool Update(double dt) override;
	void Render(double dt) override;

private:
#if _DEBUG
	json commands_json_out;

	int selectedEditZone = 0;
	int selectedEditModel = 0;
	int editType = 0; //0 = zones, 1 = models
	int selectedNewModelIndex = 0;
	bool showModelSelector = false;
	bool showPopup = false;
	std::string popupString = "";

	int subdivisionCount = 10;
	bool hasDoneEditorPreload = false;

	std::vector<Model*> allActiveModels = std::vector<Model*>(); //references to models not split into zones (done at compile time in editor)
	std::vector<std::string> allActiveModelNames = std::vector<std::string>(); //names of models loaded, pulled from metadata
	std::vector<BoundingBox*> allActiveZoneDummys = std::vector<BoundingBox*>(); //zone dummy objects that will be compiled as model parents
#endif
};

