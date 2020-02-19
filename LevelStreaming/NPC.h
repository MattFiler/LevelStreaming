#pragma once

#include "Model.h"
#include "LevelZoneGrid.h"
#include "LevelZoneTile.h"

class Waypoint;

class NPC : public Model {
public:
	friend class EditorScene;

	NPC() {
		isActive = false;
	}
	~NPC() {
		Release();
	}

	void CreateModel();
	void Update(float dt) override;
	void Render(float dt) override;

	void SetName(std::string _n) {
		npcName = _n;
	}
	std::string GetName() {
		return npcName;
	}

	void AddPathingPoint(DirectX::XMFLOAT3 _point) {
		pathingPoints.push_back(_point);
	}
	void RemovePathingPoints() {
		pathingPoints.clear();
	}
	std::vector<DirectX::XMFLOAT3> GetPathingPoints() {
		return pathingPoints;
	}

	void SetPathingSpeed(int _speed) {
		pathingSpeed = _speed;
	}
	int GetPathingSpeed() {
		return pathingSpeed;
	}

	void SetModelName(std::string _model) {
		modelName = _model;
	}
	std::string GetModelName() {
		return modelName;
	}

private:
	std::string npcName = "";
	std::string modelName = "";

	std::vector<DirectX::XMFLOAT3> pathingPoints = std::vector<DirectX::XMFLOAT3>();
	int pathingSpeed = 1;
	int currentPathingPointTarget = 0;
};