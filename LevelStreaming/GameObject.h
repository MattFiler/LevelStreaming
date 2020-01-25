#pragma once

#include "dxutils.h"
#include "GameObjectManager.h"

class GameObject
{
public:
	GameObject() = default;
	~GameObject() {
		GameObjectManager::RemoveObject(this);
		Release();
	}

	virtual void Create();
	virtual void Release();
	virtual void Update(float dt);
	virtual void Render(float dt);

	virtual void SetRotation(XMFLOAT3 _rot)
	{
		rotation = _rot;
	}
	virtual XMFLOAT3 GetRotation()
	{
		return rotation;
	}

	virtual void SetPosition(XMFLOAT3 _pos)
	{
		position = _pos;
	}
	virtual XMFLOAT3 GetPosition()
	{
		return position;
	}
	
	virtual void SetScale(float _scale)
	{
		scale = XMFLOAT3(_scale, _scale, _scale);
	}
	virtual void SetScale(XMFLOAT3 _scale)
	{
		scale = _scale;
	}
	virtual XMFLOAT3 GetScale()
	{
		return scale;
	}

	void SetActive(bool _active) 
	{
		isActive = _active;
	}
	bool GetActive() 
	{
		return isActive;
	}

	void SetInvisible(bool _invis)
	{
		isInvisible = _invis;
	}
	bool GetInvisible()
	{
		return isInvisible;
	}

	XMMATRIX GetWorldMatrix() {
		return mWorld;
	}
	XMFLOAT4X4 GetWorldMatrix4X4() {
		XMFLOAT4X4 temp;
		DirectX::XMStoreFloat4x4(&temp, mWorld);
		return temp;
	}

	void SetWorldMatrix(XMMATRIX newWorld) {
		DirectX::XMVECTOR position_v;
		DirectX::XMVECTOR rotation_v;
		DirectX::XMVECTOR scale_v;
		DirectX::XMMatrixDecompose(&scale_v, &rotation_v, &position_v, newWorld);

		DirectX::XMFLOAT3 position_i;
		DirectX::XMStoreFloat3(&position_i, position_v);
		DirectX::XMFLOAT3 rotation_i;
		DirectX::XMStoreFloat3(&rotation_i, rotation_v);
		DirectX::XMFLOAT3 scale_i;
		DirectX::XMStoreFloat3(&scale_i, scale_v);

		SetPosition(position_i);
		SetRotation(rotation_i);
		SetScale(scale_i);
	}
	void SetWorldMatrix4X4(XMFLOAT4X4 newWorld) {
		SetWorldMatrix(DirectX::XMLoadFloat4x4(&newWorld));
	}

protected:
	XMMATRIX mWorld;

	XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	bool isActive = true; //If false, won't render or update
	bool isInvisible = false; //If true, will update but not render

#ifdef _DEBUG
	ID3D11Buffer* GO_ConstantBuffer = nullptr;
	ID3D11Buffer* GO_VertexBuffer = nullptr;
	ID3D11Buffer* GO_IndexBuffer = nullptr;
	ID3D11VertexShader* GO_VertexShader = nullptr;
	ID3D11PixelShader* GO_PixelShader = nullptr;
	ID3D11InputLayout* GO_VertLayout = nullptr;
	int GO_VertCount = 0;
	int GO_IndexCount = 0;
#endif
};

