#pragma once

#include "dxutils.h"
#include "SharedModelPart.h"
#include "GameObjectManager.h"

class SharedModelBuffers {
public:
	SharedModelBuffers(std::string path_to_obj);
	~SharedModelBuffers();

	void Render(XMMATRIX mWorld);

	std::string GetFilePath() {
		return objPath;
	}

	int GetVertCount() {
		return vertexCount;
	}

	int GetUseageCount() {
		return useCount;
	}
	void AddUseage() {
		useCount++;
	}
	void RemoveUseage() {
		useCount--;
	}

private:
	Utilities dxutils = Utilities();

	ID3D11Buffer* g_pVertexBuffer = nullptr;

	ID3D11SamplerState* g_pSamplerLinear = nullptr;
	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_vertexLayout = nullptr;

	std::vector<SharedModelPart*> allModels = std::vector<SharedModelPart*>();
	std::vector<SimpleVertex> allVerts = std::vector<SimpleVertex>();

	std::string objPath = "";
	int vertexCount = 0;
	int useCount = 0;
};