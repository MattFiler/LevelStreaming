#pragma once

#include "dxutils.h"

class SharedModelBuffers;
class LevelZoneTile;
class NPC;
class Camera;
class Trigger;

/* A struct for returning refs to neighbours of a zone tile */
struct LevelZoneTileNeighbours {
	LevelZoneTile* BottomLeft = nullptr;
	LevelZoneTile* Left = nullptr;
	LevelZoneTile* TopLeft = nullptr;
	LevelZoneTile* Top = nullptr;
	LevelZoneTile* TopRight = nullptr;
	LevelZoneTile* Right = nullptr;
	LevelZoneTile* BottomRight = nullptr;
	LevelZoneTile* Bottom = nullptr;
	std::vector<LevelZoneTile*> AllNeighbours = std::vector<LevelZoneTile*>();
};

/* A level grid, containing zone tiles */
class LevelZoneGrid {
public:
	friend class LevelZoneTile;
	friend class EditorScene;

	LevelZoneGrid(DirectX::XMFLOAT2 _bl, DirectX::XMFLOAT2 _tr, int _sd) {
		Resize(_bl, _tr, _sd);
	}
	~LevelZoneGrid();

	void Resize(DirectX::XMFLOAT2 _bl, DirectX::XMFLOAT2 _tr, int _sd);

	void AddLevelModelPair(BinModelPair _m) {
		levelModels.push_back(_m);
	}
	void AddNPC(NPC* _npc) {
		levelNPCs.push_back(_npc);
	}
	void AddTrigger(Trigger* _trigger) {
		levelTriggers.push_back(_trigger);
	}

	LevelZoneTile* GetTileAtPosition(DirectX::XMFLOAT2 _p);
	LevelZoneTile* GetTileAtGridPos(DirectX::XMFLOAT2 _gp);
	LevelZoneTileNeighbours GetTileNeighbours(LevelZoneTile* _t);
	std::vector<LevelZoneTile*> GetAllTiles() {
		return levelTiles;
	}

	BinModel GetModelData(std::string model_name, LevelOfDetail model_lod);

	void TrackLoading();
	std::vector<Trigger*> GetActiveTriggers(Camera* _player);

	void ForceLoadNPCS();

private:
	SharedModelBuffers* LoadModelToLevel(BinModel model_data);

	DirectX::XMFLOAT2 bottomLeft;
	DirectX::XMFLOAT2 topRight;

	DirectX::XMFLOAT2 gridDims;
	DirectX::XMFLOAT2 tileDims;
	int subdivisionCount;

	std::vector<LevelZoneTile*> levelTiles = std::vector<LevelZoneTile*>();             //All tiles in the level
	std::vector<BinModelPair> levelModels = std::vector<BinModelPair>();                //Definitions of model data (to be pulled from our PAK)
	std::vector<SharedModelBuffers*> loadedModels = std::vector<SharedModelBuffers*>(); //Loaded model buffers
	std::vector<NPC*> levelNPCs = std::vector<NPC*>();                                  //All NPCs in the level
	std::vector<Trigger*> levelTriggers = std::vector<Trigger*>();                      //All triggers in the level
};