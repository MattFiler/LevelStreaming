#pragma once

#include "dxutils.h"
#include "Light.h"

class SharedModelPart {
public:
	SharedModelPart(LoadedModelPart _m);
	~SharedModelPart();

	void Render(XMMATRIX world);

private:
	ID3D11Buffer* g_pConstantBuffer = nullptr;
	ID3D11Buffer* g_pIndexBuffer = nullptr;

	Utilities dxutils = Utilities();

	LoadedMaterial loadedMaterial = LoadedMaterial();
	LoadedModelPart modelMetaData = LoadedModelPart();

	int indexCount = 0;
};