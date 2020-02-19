#pragma once

#include "DebugCube.h"

class BoundingBox : public DebugCube {
public:
	~BoundingBox() {
		Release();
	}

	void Create() override;

	void SetRotation(XMFLOAT3 _rot, bool _isRadians = false) override {
		rotation = DirectX::XMFLOAT3(0, 0, 0); //Bounding boxes don't currently support rotation
	}
	void SetScale(DirectX::XMFLOAT3 _scale) {
		GameObject::SetScale(_scale);
		localBottomLeft = DirectX::XMFLOAT3(-_scale.x, -_scale.y, -_scale.z);
		localTopRight = DirectX::XMFLOAT3(_scale.x, _scale.y, _scale.z);
	}

	void SetDims(DirectX::XMFLOAT3 _bl, DirectX::XMFLOAT3 _tr);
	BoundsPair GetDims() {
		BoundsPair dims = BoundsPair();
		dims.localBottomLeft = localBottomLeft;
		dims.localTopRight = localTopRight;
		return dims;
	}
	bool ContainsPoint(DirectX::XMFLOAT3 _p);

protected:
	DirectX::XMFLOAT3 localBottomLeft = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 localTopRight = DirectX::XMFLOAT3(0, 0, 0);
};