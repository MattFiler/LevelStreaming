#pragma once

#include "dxutils.h"
#include "LevelZoneGrid.h"

class Model;

/* A zone tile within the level grid */
class LevelZoneTile {
public:
	friend class EditorScene;

	/* Create the tile with 2D bottom left, top right points in world space, and local grid X,Y - also get ref to grid */
	LevelZoneTile(DirectX::XMFLOAT2 _bl, DirectX::XMFLOAT2 _tr, DirectX::XMFLOAT2 _gp, LevelZoneGrid* _g) {
		bottomLeft = _bl;
		topRight = _tr;
		gridPos = _gp;
		mainGrid = _g;
	}
	~LevelZoneTile();

	/* Is a 2D point within the tile? */
	bool Contains(DirectX::XMFLOAT2 _p) {
		if (_p.x > bottomLeft.x && _p.x < topRight.x) {
			if (_p.y > bottomLeft.y && _p.y < topRight.y) {
				return true;
			}
		}
		return false;
	}

	/* Get grid position */
	DirectX::XMFLOAT2 GetGridPos() {
		return gridPos;
	}

	/* Add a model ref to the zone tile */
	void AddModelRef(ModelPlacement _m) {
		Debug::Log("Adding model ref to tile!");
		models.push_back(_m);
	}

	void LoadTile(LevelOfDetail _lod);
	void UnloadTile();

	/* Is the tile loaded/loading? */
	bool IsTileLoaded() {
		return isLoaded;
	}
	bool IsTileLoading() {
		return isLoading;
	}
	bool IsTileLoadedOrLoading() {
		return isLoading || isLoaded;
	}

	void TrackLoading();

private:
	void LoadTileThread(LevelOfDetail _lod);

	LevelZoneGrid* mainGrid = nullptr;

	DirectX::XMFLOAT2 bottomLeft;
	DirectX::XMFLOAT2 topRight;
	DirectX::XMFLOAT2 gridPos;

	LevelOfDetail currentLOD = LevelOfDetail::LOW;

	bool isLoading = false;
	bool isLoaded = false;
	bool isPushed = false;
	std::mutex mutex;

	std::vector<ModelPlacement> models = std::vector<ModelPlacement>();
	std::vector<Model*> loadedModels = std::vector<Model*>();
};