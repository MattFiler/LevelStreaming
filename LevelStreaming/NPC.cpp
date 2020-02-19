#include "NPC.h"
#include "Waypoint.h"

/* Create base resources */
void NPC::Create()
{
	GameObject::Create();
}

/* A wrapper for model creation, to be called when swapping model data and LODs */
void NPC::CreateModel()
{
	if (!modelData) Debug::Log("Creating an NPC without its model data - check scripts!");
	if (modelData) modelData->AddUseage();
}

/* A wrapper for model deletion, to be called when swapping model data and LODs */
void NPC::ReleaseModel()
{
	if (modelData) modelData->RemoveUseage();
	modelData = nullptr;
}

/* Move the NPC if it's active */
void NPC::Update(float dt)
{
	Model::Update(dt);

	if (dxshared::pauseNPCs && pathingPoints.size() != 0) {
		currentPathingPointTarget = 0;
	}

	if (!isActive) return;
	if (dxshared::pauseNPCs || dt == 0 || pathingPoints.size() == 0) return;

	float tx = pathingPoints[currentPathingPointTarget].x - position.x;
	float ty = pathingPoints[currentPathingPointTarget].y - position.y;
	float tz = pathingPoints[currentPathingPointTarget].z - position.z;
	float length = sqrt(tx*tx + ty*ty + tz*tz);
	if (length > (dt*pathingSpeed))
	{
		position = DirectX::XMFLOAT3(position.x + (dt*pathingSpeed) * tx / length, position.y + (dt*pathingSpeed) * ty / length, position.z + (dt*pathingSpeed) * tz / length);
	}
	else
	{
		currentPathingPointTarget++;
		if (currentPathingPointTarget >= pathingPoints.size()) currentPathingPointTarget = 0;
	}
}

/* Render the NPC if it's loaded */
void NPC::Render(float dt)
{
	if (GetLOD() == LevelOfDetail::UNLOADED) return;
	Model::Render(dt);
}
