#include "Collision.h"

bool Collision::boxCollision(Vector3 A, Vector3 B, Vector3 A_radius, Vector3 B_radius)
{
	//ÉvÉåÉCÉÑÅ[
	float aXMin = A.x - A_radius.x;

	float aXMax = A.x + A_radius.x;

	float aYMin = A.y - A_radius.y;

	float aYMax = A.y + A_radius.y;

	float aZMin = A.z - A_radius.z;

	float aZMax = A.z + A_radius.z;

	//ìG
	//ïΩñ 
	float bXMin = B.x - B_radius.x;

	float bXMax = B.x + B_radius.x;

	float bYMin = B.y - B_radius.y;

	float bYMax = B.y + B_radius.y;

	float bZMin = B.z - B_radius.z;

	float bZMax = B.z + B_radius.z;

	if ((aXMin <= bXMax && aXMax >= bXMin) && (aYMin <= bYMax && aYMax >= bYMin) && (aZMin <= bZMax && aZMax >= bZMin)) {
		return true;
	}
    return false;
}
