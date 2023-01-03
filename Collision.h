#pragma once
#include "Vector3.h"
class Collision
{
public:
	//AとBの座標を入る
	//Aの半径をx,y,zの順に入れる
	//Bの半径をx,y,zの順に入れる
	bool boxCollision(Vector3 A, Vector3 B, Vector3 A_radius, Vector3 B_radius);
};

