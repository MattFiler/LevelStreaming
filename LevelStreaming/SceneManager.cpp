#include "SceneManager.h"
#include "Scene.h"

/* Destroy active scene on exit, if one is */
SceneManager::~SceneManager()
{
	if (currentSceneIndex != -1)
		availableScenes[currentSceneIndex]->Release();
}

bool SceneManager::Init()
{
	bool dxInit = dxmain::Init();

	//Parse levels JSON & load all into LevelScene
	std::fstream lvl_js("DATA\\LEVELS.JSON");
	lvl_js >> levels_json;
	for (int i = 0; i < levels_json["LEVELS"].size(); i++) {
		bool fe_level = (levels_json["LEVELS"][i]["TYPE"] == "FE_LEVEL");
		LevelScene* level_scene = new LevelScene(levels_json["LEVELS"][i]["PATH"], fe_level);
		AddScene(level_scene);
		if (fe_level) ChangeScene(i);
	}

	return dxInit;
}

/* Update the current scene, and handle swapping of scenes */
bool SceneManager::Update(double dt)
{
	//Swap scenes if requested
	if (requestedSceneIndex != currentSceneIndex)
	{
		if (currentSceneIndex != -1)
			availableScenes[currentSceneIndex]->Release();
		currentSceneIndex = requestedSceneIndex;
		availableScenes[currentSceneIndex]->Init();
	}

	//New ImGui frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	//Scene manager ImGui control
	bool open = true;
	ImGui::SetNextWindowPos(ImVec2(0, 685));
	ImGui::SetNextWindowSize(ImVec2(950, 35));
	ImGui::Begin("Editor", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

	for (int i = 0; i < availableScenes.size(); i++) {
		if (ImGui::Button(("Scene " + std::to_string(i)).c_str()))
		{
			ChangeScene(i);
		}
		ImGui::SameLine();
	}

	ImGui::End();
	
	//Update current scene
	if (currentSceneIndex != -1)
		return availableScenes[currentSceneIndex]->Update(dt);
}

/* Render the current scene (if one is set) */
void SceneManager::Render(double dt)
{
	//Clear back buffer & depth stencil view
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::CornflowerBlue);
	m_pImmediateContext->ClearDepthStencilView(g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//Render scene
	if (currentSceneIndex != -1)
		availableScenes[currentSceneIndex]->Render(dt);

	//Render ImGui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	//Present the back buffer to front buffer
	m_pSwapChain->Present(0, 0);
}
