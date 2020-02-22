#include "LevelZoneGrid.h"
#include "SharedModelBuffers.h"
#include "LevelZoneTile.h"
#include "NPC.h"
#include "Trigger.h"
#include "Camera.h"

LevelZoneGrid::~LevelZoneGrid()
{
	for (int i = 0; i < levelTiles.size(); i++) {
		delete levelTiles[i];
	}
	levelTiles.clear();
	for (int i = 0; i < loadedModels.size(); i++) {
		delete loadedModels[i];
	}
	loadedModels.clear();
	levelModels.clear();
	for (int i = 0; i < levelNPCs.size(); i++) {
		levelNPCs[i]->Release();
		delete levelNPCs[i];
	}
	levelNPCs.clear();
	for (int i = 0; i < levelTriggers.size(); i++) {
		levelTriggers[i]->Release();
		delete levelTriggers[i];
	}
	levelTriggers.clear();
}

/* Size the grid with 2D bottom left, top right points in world space, and a tile subdivision count */
void LevelZoneGrid::Resize(DirectX::XMFLOAT2 _bl, DirectX::XMFLOAT2 _tr, int _sd)
{
	bottomLeft = _bl;
	topRight = _tr;

	gridDims = DirectX::XMFLOAT2(_tr.x - _bl.x, _tr.y - _bl.y);
	tileDims = DirectX::XMFLOAT2(gridDims.x / _sd, gridDims.y / _sd);
	subdivisionCount = _sd;

	levelTiles.clear();
	for (int x = 0; x < _sd; x++) {
		for (int y = 0; y < _sd; y++) {
			LevelZoneTile* newTile = new LevelZoneTile(
				DirectX::XMFLOAT2(_bl.x + (tileDims.x * x), _bl.y + (tileDims.y * y)),
				DirectX::XMFLOAT2(_bl.x + (tileDims.x * (x + 1)), _bl.y + (tileDims.y * (y + 1))),
				DirectX::XMFLOAT2(x, y),
				this
			);
			levelTiles.push_back(newTile);
		}
	}
}

/* Get a tile at a certain 2D position */
LevelZoneTile * LevelZoneGrid::GetTileAtPosition(DirectX::XMFLOAT2 _p)
{
	for (int i = 0; i < levelTiles.size(); i++) {
		if (levelTiles[i]->Contains(_p)) {
			return levelTiles[i];
		}
	}
	return nullptr;
}

/* Get a tile at a position with in the grid */
LevelZoneTile * LevelZoneGrid::GetTileAtGridPos(DirectX::XMFLOAT2 _gp)
{
	for (int i = 0; i < levelTiles.size(); i++) {
		if (levelTiles[i]->GetGridPos().x == _gp.x && levelTiles[i]->GetGridPos().y == _gp.y) {
			return levelTiles[i];
		}
	}
	return nullptr;
}

/* Get the neighbours to a tile in the grid */
LevelZoneTileNeighbours LevelZoneGrid::GetTileNeighbours(LevelZoneTile * _t)
{
	LevelZoneTileNeighbours neighbours = LevelZoneTileNeighbours();
	DirectX::XMFLOAT2 p0 = _t->GetGridPos();
	for (int i = 0; i < levelTiles.size(); i++)
	{
		DirectX::XMFLOAT2 p1 = levelTiles[i]->GetGridPos();
		if (p1.y == p0.y - 1 && p1.x == p0.x - 1) //Bottom left
		{
			neighbours.AllNeighbours.push_back(levelTiles[i]);
			neighbours.BottomLeft = levelTiles[i];
			continue;
		}
		if (p1.y == p0.y && p1.x == p0.x - 1) //Left
		{
			neighbours.AllNeighbours.push_back(levelTiles[i]);
			neighbours.Left = levelTiles[i];
			continue;
		}
		if (p1.y == p0.y + 1 && p1.x == p0.x - 1) //Top left
		{
			neighbours.AllNeighbours.push_back(levelTiles[i]);
			neighbours.TopLeft = levelTiles[i];
			continue;
		}
		if (p1.y == p0.y + 1 && p1.x == p0.x) //Top
		{
			neighbours.AllNeighbours.push_back(levelTiles[i]);
			neighbours.Top = levelTiles[i];
			continue;
		}
		if (p1.y == p0.y + 1 && p1.x == p0.x + 1) //Top right
		{
			neighbours.AllNeighbours.push_back(levelTiles[i]);
			neighbours.TopRight = levelTiles[i];
			continue;
		}
		if (p1.y == p0.y && p1.x == p0.x + 1) //Right
		{
			neighbours.AllNeighbours.push_back(levelTiles[i]);
			neighbours.Right = levelTiles[i];
			continue;
		}
		if (p1.y == p0.y - 1 && p1.x == p0.x + 1) //Bottom right
		{
			neighbours.AllNeighbours.push_back(levelTiles[i]);
			neighbours.BottomRight = levelTiles[i];
			continue;
		}
		if (p1.y == p0.y - 1 && p1.x == p0.x) //Bottom
		{
			neighbours.AllNeighbours.push_back(levelTiles[i]);
			neighbours.Bottom = levelTiles[i];
			continue;
		}
	}
	return neighbours;
}

/* Get model BIN data given the model name and LOD */
BinModel LevelZoneGrid::GetModelData(std::string model_name, LevelOfDetail model_lod) 
{
	for (int i = 0; i < levelModels.size(); i++) {
		if (levelModels[i].name == model_name) {
			if (model_lod == LevelOfDetail::HIGH) return levelModels[i].LOD0;
			else if (model_lod == LevelOfDetail::LOW) return levelModels[i].LOD1;
		}
	}
	Debug::Log("ERROR! Requested model data for model '" + model_name + "' at LOD " + std::to_string((int)model_lod) + ", but it wasn't found!");
	return BinModel();
}

/* Keep track of load states on all tiles (should call on update) */
void LevelZoneGrid::TrackLoading()
{
	//Track zone loading
	bool noneAreLoading = true;
	for (int i = 0; i < levelTiles.size(); i++) {
		levelTiles[i]->TrackLoading();
		if (levelTiles[i]->IsTileLoading()) noneAreLoading = false;
	}

#ifndef _DEBUG
	//Load/unload NPCs depending on the active-ness of the tile they're in
	for (int i = 0; i < levelNPCs.size(); i++) {
		LevelZoneTile* tile = GetTileAtPosition(DirectX::XMFLOAT2(levelNPCs[i]->GetPosition().x, levelNPCs[i]->GetPosition().z));

		//Unload model if it doesn't match the current zone LOD
		if (tile != nullptr && levelNPCs[i]->GetLOD() != LevelOfDetail::UNLOADED && levelNPCs[i]->GetLOD() != tile->GetLOD()) {
			levelNPCs[i]->ReleaseModel();
			Debug::Log("Unloading NPC: " + levelNPCs[i]->GetName());
		}

		//Unload model if it's outside of the active tiles
		if (tile == nullptr || !tile->IsTileLoaded()) {
			if (levelNPCs[i]->GetLOD() != LevelOfDetail::UNLOADED) {
				levelNPCs[i]->ReleaseModel();
				Debug::Log("Unloading NPC: " + levelNPCs[i]->GetName());
			}
		}

		//Load model if it's in an active tile and not already at the correct LOD
		else
		{
			if (levelNPCs[i]->GetLOD() == LevelOfDetail::UNLOADED) {
				for (int x = 0; x < levelModels.size(); x++) {
					if (levelModels[x].modelName == levelNPCs[i]->GetModelName()) {
						Debug::Log("Loading NPC, with model " + levelModels[x].modelName + ", at LOD " + std::to_string((int)tile->GetLOD()) + ".");
						switch (tile->GetLOD()) {
							case LevelOfDetail::HIGH:
								levelNPCs[i]->SetData(LoadModelToLevel(levelModels[x].modelPath_LOD1, tile->GetLOD()));
								break;
							case LevelOfDetail::LOW:
								levelNPCs[i]->SetData(LoadModelToLevel(levelModels[x].modelPath_LOD2, tile->GetLOD()));
								break;
						}
						levelNPCs[i]->CreateModel();
						break;
					}
				}
			}
		}
	}
#endif

	if (!noneAreLoading) return;
	//Check to see if any model buffers aren't in use anymore - delete them if so
	std::vector<SharedModelBuffers*> loadedModelsUpdated = std::vector<SharedModelBuffers*>();
	for (int i = 0; i < loadedModels.size(); i++) {
		if (loadedModels[i]->GetUseageCount() == 0) {
			delete loadedModels[i];
			continue;
		}
		loadedModelsUpdated.push_back(loadedModels[i]);
	}
	loadedModels = loadedModelsUpdated;
}

/* Returns a vector of all currently active triggers (triggers the player has entered) */
std::vector<Trigger*> LevelZoneGrid::GetActiveTriggers(Camera* _player)
{
	std::vector<Trigger*> activeTriggers = std::vector<Trigger*>();
	for (int i = 0; i < levelTriggers.size(); i++) {
		if (levelTriggers[i]->ContainsPoint(_player->GetPosition())) activeTriggers.push_back(levelTriggers[i]);
	}
	return activeTriggers;
}

/* For debugging, load all NPCs at their highest LOD */
void LevelZoneGrid::ForceLoadNPCS()
{
	return;
	bool didLoad = false;
	for (int i = 0; i < levelNPCs.size(); i++) {
		for (int x = 0; x < levelModels.size(); x++) {
			if (levelModels[x].name == levelNPCs[i]->GetModelName()) {
				Debug::Log("Loading NPC, with model " + levelModels[x].name);
				levelNPCs[i]->SetData(LoadModelToLevel(levelModels[x].LOD0));
				levelNPCs[i]->CreateModel();
				didLoad = true;
				break;
			}
		}
		if (!didLoad) {
			Debug::Log("ERROR! Tried to load NPC with model " + levelNPCs[i]->GetModelName() + ", but it wasn't found!");
		}
		didLoad = false;
	}
}

/* Requested load of model: check our existing loaded data, and if not already loaded, load it */
SharedModelBuffers* LevelZoneGrid::LoadModelToLevel(BinModel model_data)
{
	//Return an already loaded model buffer, if it exists
	for (int i = 0; i < loadedModels.size(); i++) {
		if (loadedModels[i]->GetModelData().modelName == model_data.modelName &&
			loadedModels[i]->GetModelData().modelLOD == model_data.modelLOD) {
			Debug::Log("Pulling model from pool.");
			return loadedModels[i];
		}
	}

	//Model isn't already loaded - load it
	SharedModelBuffers* newLoadedModel = new SharedModelBuffers(model_data);
	loadedModels.push_back(newLoadedModel);
	return newLoadedModel;
}
