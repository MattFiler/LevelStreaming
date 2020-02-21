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

	//Parse levels data & load all into LevelScene
	std::ifstream fin("DATA\\LEVELS.BIN", std::ios::in | std::ios::binary);
	int file_version;
	fin.read((char*)&file_version, 4);
	int entry_count;
	fin.read((char*)&entry_count, 4);
	for (int i = 0; i < entry_count; i++)
	{
		INT8 string_len;
		fin.read((char*)&string_len, 1);
		std::string level_name = "";
		for (int x = 0; x < string_len; x++) {
			char this_char;
			fin.read((char*)&this_char, 1);
			level_name += this_char;
		}
		fin.read((char*)&string_len, 1);
		std::string level_path = "";
		for (int x = 0; x < string_len; x++) {
			char this_char;
			fin.read((char*)&this_char, 1);
			level_path += this_char;
		}
		INT16 level_type;
		fin.read((char*)&level_type, 2);

		LevelScene* level_scene = new EditorScene(level_name, level_path, (LevelType)level_type);
		AddScene(level_scene);

		if (level_type == LevelType::FE_LEVEL) ChangeScene(i);
	}
	fin.close();

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
	ImGuizmo::BeginFrame();

	//Scene manager ImGui control
	bool open = true;
	ImGui::SetNextWindowPos(ImVec2(0, 685));
	ImGui::SetNextWindowSize(ImVec2(950, 35));
	ImGui::Begin("Editor", &open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

	for (int i = 0; i < availableScenes.size(); i++) {
		if (!dynamic_cast<LevelScene*>(availableScenes.at(i))) continue; //Expensive every frame... only use for debugging
		if (ImGui::Button((dynamic_cast<LevelScene*>(availableScenes.at(i))->GetName()).c_str()))
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
