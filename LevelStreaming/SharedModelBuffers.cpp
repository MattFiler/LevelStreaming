#include "SharedModelBuffers.h"

/* Load a model and create the buffers */
SharedModelBuffers::SharedModelBuffers(BinModel model_data)
{
	//Push data for our vertex buffer, and create children index buffers
	Debug::Log("Loading model from disk.");
	modelData = model_data;
	LoadedModel _m = dxutils.LoadModelFromPAK(model_data);
	for (int i = 0; i < _m.modelParts.size(); i++) {
		for (int x = 0; x < _m.modelParts[i].compVertices.size(); x++) {
			allVerts.push_back(_m.modelParts[i].compVertices[x]);
		}
		allModels.push_back(new SharedModelPart(_m.modelParts[i]));
	}

	//Create vertex buffer 
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	vertexCount = allVerts.size();
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = allVerts.data();
	HR(dxshared::m_pDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer));

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HR(dxshared::m_pDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear));

	//Compile the vertex shader
	ID3DBlob* pVSBlob = nullptr;
	Utilities dxutils = Utilities();
	HR(dxutils.CompileShaderFromFile(L"DATA/SHADERS/ObjectShader.fx", "VS", "vs_4_0", &pVSBlob));

	//Create the vertex shader
	HR(dxshared::m_pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &m_vertexShader));

	//Define the input layout
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);

	//Create the input layout
	HR(dxshared::m_pDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &m_vertexLayout));
	pVSBlob->Release();

	//Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
	HR(dxutils.CompileShaderFromFile(L"DATA/SHADERS/ObjectShader.fx", "PS", "ps_4_0", &pPSBlob));

	//Create the pixel shader
	HR(dxshared::m_pDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &m_pixelShader));
	pPSBlob->Release();
}

/* Clear all buffers */
SharedModelBuffers::~SharedModelBuffers()
{
	Memory::SafeRelease(g_pVertexBuffer);
	Memory::SafeRelease(m_vertexShader);
	Memory::SafeRelease(m_pixelShader);

	for (int i = 0; i < allModels.size(); i++) {
		delete allModels[i];
	}
	allModels.clear();
}

/* Render the model parts */
void SharedModelBuffers::Render(XMMATRIX mWorld)
{
	if (vertexCount == 0) return;
	if (allModels.size() == 0) return;

	//Set shaders to use
	dxshared::m_pImmediateContext->VSSetShader(m_vertexShader, nullptr, 0);
	dxshared::m_pImmediateContext->PSSetShader(m_pixelShader, nullptr, 0);

	//Set vertex buffer 
	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;
	dxshared::m_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	//Set sampler
	dxshared::m_pImmediateContext->PSSetSamplers(0, 1, &g_pSamplerLinear);

	//Set input layout (submeshes don't use GO render state, so we can do this here)
	dxshared::m_pImmediateContext->IASetInputLayout(m_vertexLayout);

	//Render each model part
	for (int i = 0; i < allModels.size(); i++) {
		allModels[i]->Render(mWorld);
	}
}
