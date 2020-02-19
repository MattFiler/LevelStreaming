#pragma once

#include "BoundingBox.h"

class Spawnpoint : public BoundingBox {
public:
	~Spawnpoint() {
		Release();
	}
	void Create() override;
};