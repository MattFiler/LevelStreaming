#include "BoundingBox2D.h"

/* Create the bounding box (if in debug this allows us to show the box in editor) */
void BoundingBox2D::Create()
{
	SetTexture("boundingbox2d");
	DebugCube::Create();
}

/* Is a given point within the bounding box in 2D? */
bool BoundingBox2D::ContainsPoint(DirectX::XMFLOAT3 _p)
{
	if (_p.x > (position.x + localBottomLeft.x) && _p.x < (position.x + localTopRight.x)) {
		if (_p.z > (position.z + localBottomLeft.z) && _p.z < (position.z + localTopRight.z)) {
			return true;
		}
	}

	return false;
}
