#pragma once

#include "DebugCube.h"

class Spawnpoint : public DebugCube {
public:
	~Spawnpoint() {
		Release();
	}
	void Create() override;
};