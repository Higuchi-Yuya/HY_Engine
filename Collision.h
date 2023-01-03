#pragma once
#include "Vector3.h"
class Collision
{
public:
	//A‚ÆB‚ÌÀ•W‚ğ“ü‚é
	//A‚Ì”¼Œa‚ğx,y,z‚Ì‡‚É“ü‚ê‚é
	//B‚Ì”¼Œa‚ğx,y,z‚Ì‡‚É“ü‚ê‚é
	bool boxCollision(Vector3 A, Vector3 B, Vector3 A_radius, Vector3 B_radius);
};

