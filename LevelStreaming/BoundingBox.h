#pragma once

#include "GameObject.h"

class BoundingBox : public GameObject {
public:
	~BoundingBox() {
		Release();
	}

	void Create() override;
	void Release() override;
	void Update(float dt) override;
	void Render(float dt) override;

	void SetDims(DirectX::XMFLOAT3 _bl, DirectX::XMFLOAT3 _tr);
	bool ContainsPoint(DirectX::XMFLOAT3 _p);

private:
	DirectX::XMFLOAT3 localBottomLeft = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 localTopRight = DirectX::XMFLOAT3(0, 0, 0);
};