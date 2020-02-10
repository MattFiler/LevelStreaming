#pragma once

#include "Model.h"
#include "LevelZoneGrid.h"
#include "LevelZoneTile.h"

class NPC : public Model {
public:
	~NPC() {
		Release();
	}

	void Create() override;
	void Release() override;
	void Update(float dt) override;
	void Render(float dt) override;

	void SetName(std::string& _n) {
		npcName = _n;
	}

private:
	std::string npcName = "";
};