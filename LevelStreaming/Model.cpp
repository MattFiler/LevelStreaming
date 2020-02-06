#include "Model.h"

/* Create all model parts */
void Model::Create()
{
	GameObject::Create();
	if (!modelData) Debug::Log("Creating a model without its data - check scripts!");
	modelData->AddUseage();
}

/* Release all model parts */
void Model::Release()
{
	GameObject::Release();
	modelData->RemoveUseage();
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