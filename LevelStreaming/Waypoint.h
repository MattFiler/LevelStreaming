#pragma once

#include "BoundingBox.h"

class Waypoint : public BoundingBox {
public:
	~Waypoint() {
		Release();
	}
	void Create() override;
};