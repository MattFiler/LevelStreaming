#include "Spawnpoint.h"

/* Create the bounding box (if in debug this allows us to show the box in editor) */
void Spawnpoint::Create()
{
	SetTexture("spawn");
	DebugCube::Create();
}