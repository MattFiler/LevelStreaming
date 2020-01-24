#pragma once

#include <vector>

class GameObject;
class Light;
class Model;
class BoundingBox;

class GameObjectManager
{
public:
	GameObjectManager() = default;
	~GameObjectManager() {
		Release();
	}

	static void AddObject(GameObject* _aLight) {
		allGameObjects.push_back(_aLight);
	}
	static void RemoveObject(GameObject* _aLight);

	static std::vector<Light*> GetLights();
	static std::vector<Model*> GetModels();
	static std::vector<BoundingBox*> GetBoundingBoxes();

	static void Create();
	static void Release();
	static void Update(float dt);
	static void Render(float dt);

private:
	static std::vector<GameObject*> allGameObjects;
};

