#include "LevelZoneTile.h"
#include "LevelZoneGrid.h"
#include "Model.h"

LevelZoneTile::~LevelZoneTile()
{
	for (int i = 0; i < loadedModels.size(); i++) {
		GameObjectManager::RemoveObject(loadedModels[i]);
		delete loadedModels[i];
	}
	loadedModels.clear();
	models.clear();
}

/* Load the zone tile */
void LevelZoneTile::LoadTile(LevelOfDetail _lod)
{
	if (IsTileLoaded() && currentLOD == _lod) return;
	if (IsTileLoading()) return;
	Debug::Log("MAIN THREAD: Starting load of zone (X:" + std::to_string((int)gridPos.x) + ", Y:" + std::to_string((int)gridPos.y) + "), at LOD " + std::to_string((int)_lod));

	currentLOD = _lod;
	isLoading = true;
	isPushed = false;

	std::thread* load_models = new std::thread([_lod, this] { LoadTileThread(_lod); });
}

/* Unload the zone tile (if forceAll, all unload, else, only old LODs unload) */
void LevelZoneTile::UnloadTile()
{
	if (!IsTileLoaded() || IsTileLoading()) return;
	
	int purgeCount = 0;
	for (int i = 0; i < loadedModels.size(); i++)
	{
		GameObjectManager::RemoveObject(loadedModels[i]);
		purgeCount++;
	}
	loadedModels.clear();
	Debug::Log("MAIN THREAD: Purged " + std::to_string(purgeCount) + " entities from zone (X:" + std::to_string((int)gridPos.x) + ", Y:" + std::to_string((int)gridPos.y) + ")");

	isLoaded = false;
	isPushed = false;
}

/* Keep track of loading, and push all GOs to the GO manager if loaded */
void LevelZoneTile::TrackLoading()
{
	if (!isLoading && isLoaded && !isPushed) {
		UnloadTile(); //Call this here to clear out any old LODs
		loadedModels = loadedModelsQueue;
		loadedModelsQueue.clear();
		for (int x = 0; x < loadedModels.size(); x++)
		{
			GameObjectManager::AddObject(loadedModels.at(x));
		}
		Debug::Log("MAIN THREAD: Completed load of " + std::to_string(loadedModels.size()) + " entities in zone (X:" + std::to_string((int)gridPos.x) + ", Y:" + std::to_string((int)gridPos.y) + ")");
		isLoaded = true;
		isPushed = true;
	}
}

/* Load the contents of this zone tile */
void LevelZoneTile::LoadTileThread(LevelOfDetail _lod)
{
	mutex.lock();
	loadedModelsQueue.clear();
	for (int i = 0; i < models.size(); i++)
	{
		for (int x = 0; x < mainGrid->levelModels.size(); x++)
		{
			if (mainGrid->levelModels[x].name == models.at(i).modelName)
			{
				Debug::Log("LOAD THREAD: Creating instance of model: " + models.at(i).modelName);
				Model* new_model = new Model();
				switch (_lod) {
				case LevelOfDetail::HIGH:
					new_model->SetData(mainGrid->LoadModelToLevel(mainGrid->levelModels[x].LOD0));
					break;
				case LevelOfDetail::LOW:
					new_model->SetData(mainGrid->LoadModelToLevel(mainGrid->levelModels[x].LOD1));
					break;
				}
				new_model->SetPosition(models.at(i).position);
				new_model->SetRotation(models.at(i).rotation);
				new_model->SetScale(models.at(i).scale);
				new_model->Create();
				loadedModelsQueue.push_back(new_model);
				break;
			}
		}
	}
	isLoading = false;
	isLoaded = true;
	mutex.unlock();
}
