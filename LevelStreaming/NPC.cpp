#include "NPC.h"

void NPC::Create() { }

/* A wrapper for model creation, to be called when swapping model data and LODs */
void NPC::CreateModel()
{
	Model::Create();
	isActive = true;
}

void NPC::Release()
{
	Model::Release();
}

/* Move the NPC if it's active */
void NPC::Update(float dt)
{
	if (GetLOD() == LevelOfDetail::UNLOADED) return;
	Model::Update(dt);

	if (!isActive) return;
	if (dt == 0) return;
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
