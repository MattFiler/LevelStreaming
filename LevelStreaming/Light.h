#pragma once

#include "GameObject.h"

class Light : public GameObject
{
public:
	~Light() {
		Release();
	}

	void SetColour(XMFLOAT4 _colour) {
		lightColour = _colour;
	}
	XMFLOAT4 GetColour() {
		return lightColour;
	}

	void SetIntensity(float _intense) {
		lightIntensity = _intense;
	}
	float GetIntensity() {
		return lightIntensity;
	}

private:
	XMFLOAT4 lightColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	float lightIntensity = 0.6f;
};