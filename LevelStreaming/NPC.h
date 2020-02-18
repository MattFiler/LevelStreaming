#pragma once

#include "Model.h"
#include "LevelZoneGrid.h"
#include "LevelZoneTile.h"

class NPC : public Model {
public:
	NPC() {
		isActive = false;
	}
	~NPC() {
		Release();
	}

	void Create() override;
	void CreateModel();
	void Release() override;
	void Update(float dt) override;
	void Render(float dt) override;

	void SetName(std::string _n) {
		npcName = _n;
	}

	void AddPathingPoint(DirectX::XMFLOAT3 _point) {
		pathingPoints.push_back(_point);
	}

	void SetPathingSpeed(float _speed) {
		pathingSpeed = _speed;
	}
	float GetPathingSpeed() {
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
	float pathingSpeed = 1.0f;
	int currentPathingPointTarget = 0;
};