#pragma once

#include "BoundingBox.h"

class Trigger : public BoundingBox {
public:
	~Trigger() {
		Release();
	}
	void Create() override;
};