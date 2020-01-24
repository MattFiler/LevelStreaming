#include "BoundingBox.h"

/* Create resources */
void BoundingBox::Create()
{
	GameObject::Create();
}

/* Release resources */
void BoundingBox::Release()
{
	GameObject::Release();
}

/* Update resources */
void BoundingBox::Update(float dt)
{
	GameObject::Update(dt);
}

/* Render resources */
void BoundingBox::Render(float dt)
{
	GameObject::Render(dt);
}

/* Set the bottom left and top right corners of the bounding cube in world space */
void BoundingBox::SetDims(DirectX::XMFLOAT3 _bl, DirectX::XMFLOAT3 _tr)
{
	float x_out = ((_tr.x - _bl.x) / 2);
	float y_out = ((_tr.y - _bl.y) / 2);
	float z_out = ((_tr.z - _bl.z) / 2);
	position = DirectX::XMFLOAT3(_bl.x + x_out, _bl.y + y_out, _bl.z + z_out); //Set GameObject position to midpoint of both points

	localBottomLeft = DirectX::XMFLOAT3(-x_out, -y_out, -z_out);
	localTopRight = DirectX::XMFLOAT3(x_out, y_out, z_out);
}

/* Is a point inside the bounding box in world space? */
bool BoundingBox::ContainsPoint(DirectX::XMFLOAT3 _p)
{
	if (_p.x > (position.x + localBottomLeft.x) && _p.x < (position.x + localTopRight.x)) {
		if (_p.y > (position.y + localBottomLeft.y) && _p.y < (position.y + localTopRight.y)) {
			if (_p.z > (position.z + localBottomLeft.z) && _p.z < (position.z + localTopRight.z)) {
				return true;
			}
		}
	}

	return false;
}
