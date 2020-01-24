#pragma once

#include "dxutils.h"

#include "Cube.h"
#include "Model.h"
#include "Camera.h"
#include "Light.h"
#include "BoundingBox.h"

#include "nlohmann/json.hpp"
using json = nlohmann::json;

class Scene
{
public:
	virtual void Init() = 0;
	virtual void Release() = 0;

	virtual bool Update(double dt) = 0;
	virtual void Render(double dt) = 0;
};

