#include "Model.h"

/* Create all model parts */
void Model::Create()
{
	GameObject::Create();
}

/* Release all model parts */
void Model::Release()
{
	GameObject::Release();
}

/* Update all model parts */
void Model::Update(float dt)
{
	GameObject::Update(dt);
}

/* Render all model parts */
void Model::Render(float dt)
{
	GameObject::Render(dt);

	if (!isActive) return;
	if (isInvisible) return;
	
	if (modelData) modelData->Render(mWorld);
}