#include "Model.h"

/* Add useage to model data */
void Model::Create()
{
	GameObject::Create();
	if (!modelData) Debug::Log("Creating a model without its data - check scripts!");
	modelData->AddUseage();
}

/* Remove useage from model data */
void Model::Release()
{
	GameObject::Release();
	modelData->RemoveUseage();
}

/* Render all model parts */
void Model::Render(float dt)
{
	GameObject::Render(dt);

	if (!isActive) return;
	if (isInvisible) return;
	
	if (modelData) modelData->Render(mWorld);
}