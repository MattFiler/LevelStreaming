#pragma once

#include "BoundingBox.h"

class BoundingBox2D : public BoundingBox {
public:
	~BoundingBox2D() {
		Release();
	}

	//Remember when calling GetDims() that Y doesn't count!
	bool ContainsPoint(DirectX::XMFLOAT3 _p);

private:

};