#pragma once

#include "CollisionPrimitive.h"


class Collision
{
public:
	// 球と平面の当たり判定
	static bool CheckSphere2Plane(const Sphere& sphere, const Plane& plane, Vector3* inter = nullptr);

	// 球と球
	static bool CheckSphere2Sphere(const Sphere& sphereA, const Sphere& sphereB, Vector3* inter = nullptr, Vector3* reject = nullptr);
	static bool CheckSphere2SphereOiOi(const Sphere& sphereA, const Sphere& sphereB, Vector3* inter = nullptr, Vector3* reject = nullptr);

	// 点と三角形の最近接点を求める
	static void ClosestPtPoint2Triangle(const Vector3& point, const Triangle& triangle, Vector3* closest);

	// 球と三角形の当たりチェック
	static bool CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, Vector3* inter = nullptr, Vector3* reject = nullptr);

	// レイと平面の当たり判定
	static bool CheckRay2Plane(const Ray& ray, const Plane& plane, float* distance = nullptr, Vector3* inter = nullptr);

	// レイと法線付き三角形の当たり判定
	static bool CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance = nullptr, Vector3* inter = nullptr);

	// レイと球の当たり判定
	static bool CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float* distance = nullptr, Vector3* inter = nullptr);

	// 球とAABBの当たり判定
	static bool CheckSphere2AABB(const Sphere& sphere, const WorldTransform& transA, Vector3* inter = nullptr, Vector3* reject = nullptr);

	// AABBの当たり判定
	static bool CheckAABB(const WorldTransform& transA, const WorldTransform& transB);

	// OBBの当たり判定
	static bool CheckOBB(const WorldTransform& obbA, const WorldTransform& obbB);

	// OBB用の分離軸に投影された軸成分から投影線分長を算出
	static float LenSegOnSeparateAxis(Vector3* Sep, Vector3* e1, Vector3* e2, Vector3* e3 = 0);
};

