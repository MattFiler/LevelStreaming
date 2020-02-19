#pragma once

#include "LevelScene.h"
#include "Waypoint.h"

class EditorScene : public LevelScene
{
public:
	EditorScene(std::string levelName, std::string levelPath, LevelType levelType);
	~EditorScene() {
		Release();
	}

	void Init() override;
	void Release() override;
	bool Update(double dt) override;
	void Render(double dt) override;

private:
#if _DEBUG
	json commands_json_out;

	int currentEditorMode = 0; //0 = npcs, 1 = triggers, 2 = models, 3 = player spawn

	int selectedEditModel = 0;
	int selectedNewModelIndex = 0;
	int selectedEditNPC = 0;
	int selectedEditTrigger = 0;

	int subdivisionCount = 10;

	bool hasDoneEditorPreload = false;
	bool showModelSelector = false;
	bool showNpcModelSelector = false;
	bool showNpcEditor = false;
	bool showPopup = false;
	std::string popupString = "";

	Spawnpoint* playerSpawn = nullptr;

	std::vector<Waypoint*> waypointMarkers = std::vector<Waypoint*>(); //waypoint markers for visuals when editing npc paths
	std::vector<Model*> allActiveModels = std::vector<Model*>(); //references to models not split into zones (done at compile time in editor)
	std::vector<std::string> allActiveModelNames = std::vector<std::string>(); //names of models loaded, pulled from metadata
#endif
};

