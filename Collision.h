#pragma once
#include "Vector3.h"
class Collision
{
public:
	//A��B�̍��W�����
	//A�̔��a��x,y,z�̏��ɓ����
	//B�̔��a��x,y,z�̏��ɓ����
	bool boxCollision(Vector3 A, Vector3 B, Vector3 A_radius, Vector3 B_radius);
};

