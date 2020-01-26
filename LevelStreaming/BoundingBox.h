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

	void SetRotation(XMFLOAT3 _rot, bool _isRadians = false) override {
		rotation = DirectX::XMFLOAT3(0, 0, 0); //Bounding boxes don't currently support rotation
	}
	void SetScale(DirectX::XMFLOAT3 _scale) {
		GameObject::SetScale(_scale);
		localBottomLeft = DirectX::XMFLOAT3(-_scale.x, -_scale.y, -_scale.z);
		localTopRight = DirectX::XMFLOAT3(_scale.x, _scale.y, _scale.z);
	}

	void ShowVisual(bool shouldShow) {
#ifdef _DEBUG
		showBounds = shouldShow;
#endif
	}

	void SetDims(DirectX::XMFLOAT3 _bl, DirectX::XMFLOAT3 _tr);
	BoundsPair GetDims() {
		BoundsPair dims = BoundsPair();
		dims.localBottomLeft = localBottomLeft;
		dims.localTopRight = localTopRight;
		return dims;
	}
	bool ContainsPoint(DirectX::XMFLOAT3 _p);

private:
	DirectX::XMFLOAT3 localBottomLeft = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 localTopRight = DirectX::XMFLOAT3(0, 0, 0);

#ifdef _DEBUG
	bool showBounds = false;
	ID3D11Buffer* GO_ConstantBuffer = nullptr;
	ID3D11Buffer* GO_VertexBuffer = nullptr;
	ID3D11Buffer* GO_IndexBuffer = nullptr;
	ID3D11SamplerState* g_pSamplerLinear = nullptr;
	ID3D11ShaderResourceView* materialTexture = nullptr;
	ID3D11VertexShader* GO_VertexShader = nullptr;
	ID3D11PixelShader* GO_PixelShader = nullptr;
	ID3D11InputLayout* GO_VertLayout = nullptr;
	int GO_VertCount = 0;
	int GO_IndexCount = 0;
#endif
};