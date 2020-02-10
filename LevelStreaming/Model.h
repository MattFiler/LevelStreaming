#pragma once

#include "dxutils.h"
#include "GameObject.h"
#include "SharedModelBuffers.h"

class Model : public GameObject {
public:
	~Model() {
		Release();
	}

	void Create() override;
	void Release() override;
	void Update(float dt) override;
	void Render(float dt) override;

	void SetData(SharedModelBuffers* _m) {
		modelData = _m;
	}

	LevelOfDetail GetLOD() {
		if (modelData) return modelData->GetLOD();
		return LevelOfDetail::UNLOADED;
	}

protected:
	SharedModelBuffers* modelData = nullptr;
};