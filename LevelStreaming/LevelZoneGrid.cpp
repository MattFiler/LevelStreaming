#include "LevelZoneGrid.h"
#include "SharedModelBuffers.h"
#include "LevelZoneTile.h"

/* Create the grid with 2D bottom left, top right points in world space, and a tile subdivision count */
LevelZoneGrid::LevelZoneGrid(DirectX::XMFLOAT2 _bl, DirectX::XMFLOAT2 _tr, int _sd)
{
	bottomLeft = _bl;
	topRight = _tr;

	gridDims = DirectX::XMFLOAT2(_tr.x - _bl.x, _tr.y - _bl.y);
	tileDims = DirectX::XMFLOAT2(gridDims.x / _sd, gridDims.y / _sd);
	subdivisionCount = _sd;

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

/* Keep track of load states on all tiles (should call on update) */
void LevelZoneGrid::TrackLoading()
{
	for (int i = 0; i < levelTiles.size(); i++) {
		levelTiles[i]->TrackLoading();
	}
}

/* Requested load of model: check our existing loaded data, and if not already loaded, load it */
SharedModelBuffers * LevelZoneGrid::LoadModelToLevel(std::string model_path)
{
	//Return an already loaded model buffer, if it exists
	for (int i = 0; i < loadedModels.size(); i++) {
		if (loadedModels[i]->GetFilePath() == model_path) {
			return loadedModels[i];
		}
	}

	//Model isn't already loaded - load it
	SharedModelBuffers* newLoadedModel = new SharedModelBuffers(model_path);
	loadedModels.push_back(newLoadedModel);
	return newLoadedModel;
}
