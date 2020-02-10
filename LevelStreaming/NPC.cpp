#include "NPC.h"

void NPC::Create()
{
	Model::Create();
}

void NPC::Release()
{
	Model::Release();
}

void NPC::Update(float dt)
{
	if (GetLOD() == LevelOfDetail::UNLOADED) return;
	Model::Update(dt);
}

void NPC::Render(float dt)
{
	if (GetLOD() == LevelOfDetail::UNLOADED) return;
	Model::Render(dt);
}
