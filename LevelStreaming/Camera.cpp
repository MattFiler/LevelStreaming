#include "Camera.h"
#include <fstream>
#include <vector>

/* Update the Camera */
void Camera::Update(float dt)
{
	GameObject::Update(dt);

	if (!isActive) return;

	camTarget = DirectX::XMLoadFloat3(&position) + XMVector3Normalize(XMVector3TransformCoord(DefaultForward, XMMatrixRotationRollPitchYaw(rotation.x, rotation.z, rotation.y)));
	dxshared::mView = XMMatrixLookAtLH(DirectX::XMLoadFloat3(&position), camTarget, camUp);

	if (isLocked) return;
	double moveSpeed = dt;
	if (InputHandler::KeyPressed(WindowsKey::SHIFT)) {
		moveSpeed *= 20;
	}
	if (InputHandler::KeyPressed(WindowsKey::W)) {
		GameObject::SetPosition(XMFLOAT3(GameObject::GetPosition().x, GameObject::GetPosition().y, GameObject::GetPosition().z - moveSpeed));
	}
	if (InputHandler::KeyPressed(WindowsKey::S)) {
		GameObject::SetPosition(XMFLOAT3(GameObject::GetPosition().x, GameObject::GetPosition().y, GameObject::GetPosition().z + moveSpeed));
	}
	if (InputHandler::KeyPressed(WindowsKey::A)) {
		GameObject::SetPosition(XMFLOAT3(GameObject::GetPosition().x + moveSpeed, GameObject::GetPosition().y, GameObject::GetPosition().z));
	}
	if (InputHandler::KeyPressed(WindowsKey::D)) {
		GameObject::SetPosition(XMFLOAT3(GameObject::GetPosition().x - moveSpeed, GameObject::GetPosition().y, GameObject::GetPosition().z));
	}
	if (InputHandler::KeyPressed(WindowsKey::Z)) {
		GameObject::SetPosition(XMFLOAT3(GameObject::GetPosition().x, GameObject::GetPosition().y + moveSpeed, GameObject::GetPosition().z));
	}
	if (InputHandler::KeyPressed(WindowsKey::X)) {
		GameObject::SetPosition(XMFLOAT3(GameObject::GetPosition().x, GameObject::GetPosition().y - moveSpeed, GameObject::GetPosition().z));
	}
	if (InputHandler::KeyPressed(WindowsKey::Q)) {
		GameObject::SetRotation(XMFLOAT3(GameObject::GetRotation().x, GameObject::GetRotation().y, GameObject::GetRotation().z - dt), true);
	}
	if (InputHandler::KeyPressed(WindowsKey::E)) {
		GameObject::SetRotation(XMFLOAT3(GameObject::GetRotation().x, GameObject::GetRotation().y, GameObject::GetRotation().z + dt), true);
	}
	if (InputHandler::KeyPressed(WindowsKey::F)) {
		GameObject::SetRotation(XMFLOAT3(GameObject::GetRotation().x - dt, GameObject::GetRotation().y, GameObject::GetRotation().z), true);
	}
	if (InputHandler::KeyPressed(WindowsKey::V)) {
		GameObject::SetRotation(XMFLOAT3(GameObject::GetRotation().x + dt, GameObject::GetRotation().y, GameObject::GetRotation().z), true);
	}
}
