#include "TestScene.h"

/* Init the objects in the scene */
void TestScene::Init()
{
	main_cam = Camera();
	GameObjectManager::AddObject(&main_cam);
	GameObjectManager::Create();
	main_cam.SetLocked(false);
}

/* Release the objects in the scene */
void TestScene::Release()
{
	GameObjectManager::Release();
}

/* Update the objects in the scene */
bool TestScene::Update(double dt)
{
	bool open = true;
	bool camLock = main_cam.GetLocked();

	ImGui::SetNextWindowPos(ImVec2(950, 0));
	ImGui::SetNextWindowSize(ImVec2(330, 720));
	ImGui::Begin("Controls", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

	ImGui::Text("Scene Controls");
	ImGui::Separator();
	ImGui::Checkbox("Lock Camera", &camLock);
#ifdef _DEBUG
	ImGui::SameLine();
	ImGui::Checkbox("Enable Debugging", &dxshared::enableDebug);
#endif

	ImGui::Separator();
	ImGui::SliderFloat("Ambient R", &dxshared::ambientLightColour.x, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient G", &dxshared::ambientLightColour.y, 0.0f, 1.0f);
	ImGui::SliderFloat("Ambient B", &dxshared::ambientLightColour.z, 0.0f, 1.0f);
	ImGui::Separator();

	if (ImGui::Button("Load Zone 1"))
	{
		Debug::Log("LoadZone1");
	}
	if (ImGui::Button("Load Zone 2"))
	{
		Debug::Log("LoadZone2");
	}

	ImGui::End();

	main_cam.SetLocked(camLock);
	GameObjectManager::Update(dt);

	return true;
}

/* Render the objects in the scene */
void TestScene::Render(double dt)
{
	GameObjectManager::Render(dt);
}
