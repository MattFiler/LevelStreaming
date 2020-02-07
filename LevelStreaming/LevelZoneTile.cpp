#include "LevelZoneTile.h"
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
void LevelZoneTile::LoadTile()
{
	if (IsTileLoaded()) return;
	isLoading = true;
	isPushed = false;
	std::thread* load_models = new std::thread([this] { LoadTileThread(); });
}

/* Unload the zone tile */
void LevelZoneTile::UnloadTile()
{
	if (!IsTileLoaded()) return;

	//Unload zone data structs
	for (int i = 0; i < loadedModels.size(); i++)
	{
		GameObjectManager::RemoveObject(loadedModels[i]);
	}
	loadedModels.clear();
	isLoaded = false;
	isPushed = false;

	//Now the bulky bit, check to see if any model buffers aren't in use anymore - delete them if so
	std::vector<SharedModelBuffers*> loadedModelsUpdated = std::vector<SharedModelBuffers*>();
	for (int i = 0; i < mainGrid->loadedModels.size(); i++) {
		if (mainGrid->loadedModels[i]->GetUseageCount() == 0) {
			delete mainGrid->loadedModels[i];
			continue;
		}
		loadedModelsUpdated.push_back(mainGrid->loadedModels[i]);
	}
	mainGrid->loadedModels = loadedModelsUpdated;
}

/* Keep track of loading, and push all GOs to the GO manager if loaded */
void LevelZoneTile::TrackLoading()
{
	if (isLoaded && !isPushed) {
		for (int x = 0; x < loadedModels.size(); x++)
		{
			GameObjectManager::AddObject(loadedModels.at(x));
		}
		isPushed = true;
	}
}

/* Load the contents of this zone tile */
void LevelZoneTile::LoadTileThread()
{
	for (int i = 0; i < models.size(); i++)
	{
		for (int x = 0; x < mainGrid->levelModels.size(); x++)
		{
			if (mainGrid->levelModels[x].modelName == models.at(i).modelName)
			{
				Debug::Log("Loading model: " + models.at(i).modelName);
				Model* new_model = new Model();
				new_model->SetData(mainGrid->LoadModelToLevel(mainGrid->levelModels[x].modelPath));
				new_model->SetPosition(models.at(i).position);
				new_model->SetRotation(models.at(i).rotation);
				new_model->SetScale(models.at(i).scale);
				new_model->Create();
				loadedModels.push_back(new_model);
				break;
			}
		}
	}
	mutex.lock();
	isLoading = false;
	isLoaded = true;
	mutex.unlock();
}
