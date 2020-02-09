#pragma once

#include "dxutils.h"

class SharedModelBuffers;
class LevelZoneTile;

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
	~LevelZoneGrid() {
		for (int i = 0; i < levelTiles.size(); i++) {
			delete levelTiles[i];
		}
		levelTiles.clear();
		for (int i = 0; i < loadedModels.size(); i++) {
			delete loadedModels[i];
		}
		loadedModels.clear();
		levelModels.clear();
	}

	void Resize(DirectX::XMFLOAT2 _bl, DirectX::XMFLOAT2 _tr, int _sd);

	void AddLevelModel(ModelDef _m) {
		levelModels.push_back(_m);
	}

	LevelZoneTile* GetTileAtPosition(DirectX::XMFLOAT2 _p);
	LevelZoneTile* GetTileAtGridPos(DirectX::XMFLOAT2 _gp);
	LevelZoneTileNeighbours GetTileNeighbours(LevelZoneTile* _t);
	std::vector<LevelZoneTile*> GetAllTiles() {
		return levelTiles;
	}

	void TrackLoading();

private:
	SharedModelBuffers* LoadModelToLevel(std::string model_path, LevelOfDetail lod);

	DirectX::XMFLOAT2 bottomLeft;
	DirectX::XMFLOAT2 topRight;

	DirectX::XMFLOAT2 gridDims;
	DirectX::XMFLOAT2 tileDims;
	int subdivisionCount;

	std::vector<LevelZoneTile*> levelTiles = std::vector<LevelZoneTile*>();
	std::vector<ModelDef> levelModels = std::vector<ModelDef>();
	std::vector<SharedModelBuffers*> loadedModels = std::vector<SharedModelBuffers*>();
};