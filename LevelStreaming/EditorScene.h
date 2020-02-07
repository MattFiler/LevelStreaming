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

	int selectedEditModel = 0;
	int selectedNewModelIndex = 0;

	int subdivisionCount = 10;

	bool hasDoneEditorPreload = false;
	bool showModelSelector = false;
	bool showPopup = false;
	std::string popupString = "";

	std::vector<Model*> allActiveModels = std::vector<Model*>(); //references to models not split into zones (done at compile time in editor)
	std::vector<std::string> allActiveModelNames = std::vector<std::string>(); //names of models loaded, pulled from metadata
#endif
};

