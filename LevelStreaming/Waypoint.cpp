#include "Waypoint.h"

/* Create the bounding box (if in debug this allows us to show the box in editor) */
void Waypoint::Create()
{
	SetTexture("waypoint");
	DebugCube::Create();
}