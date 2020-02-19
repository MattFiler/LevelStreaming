#pragma once

#include "DebugCube.h"

class Waypoint : public DebugCube {
public:
	~Waypoint() {
		Release();
	}
	void Create() override;
};