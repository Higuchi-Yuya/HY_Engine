#pragma once
#include "Vector3.h"
#include "Vector4.h"

// 球
struct Sphere
{
	// 中心座標
	Vector3 center = { 0,0,0 };

	// 半径
	float radius = 1.0f;
};

// 平面
struct Plane
{
	// 法線ベクトル
	Vector3 normal = { 0,1,0 };

	// 原点(0,0,0)からの距離
	float distance = 0.0f;
};

// 法線付き三角形（時計回りが表面）
class Triangle
{
public:
	// 頂点座標３つ
	Vector3 p0;
	Vector3 p1;
	Vector3 p2;

	// 法線ベクトル
	Vector3 normal;

	void ComputeNormal();
};

struct Ray
{
	// 始点座標
	Vector3 start = { 0,0,0 };

	// 方向
	Vector3 dir = { 1,0,0 };
};

