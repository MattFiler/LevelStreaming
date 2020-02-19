#include "Trigger.h"

/* Create the bounding box (if in debug this allows us to show the box in editor) */
void Trigger::Create()
{
	SetTexture("trigger");
	DebugCube::Create();
}