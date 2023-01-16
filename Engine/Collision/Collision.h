#pragma once
#include "CollisionPrimitive.h"

class Collision
{
public:
	// 球と平面の当たり判定
	static bool CheckSphere2Plane(const Sphere& sphere, const Plane& plane, Vector3* inter = nullptr);

	// 点と三角形の最近接点を求める
	static void ClosestPtPoint2Triangle(const Vector3& point, const Triangle& triangle, Vector3* closest);

	// 球と三角形の当たりチェック
	static bool CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, Vector3* inter = nullptr);


};

