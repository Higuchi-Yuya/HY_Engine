#pragma once

#include "WorldTransform.h"

// 球
struct Sphere
{
	// 中心座標
	Vector3 center = { 0,0,0 };

	// 半径
	float radius = 1.0f;
};

// ボックス
struct Box
{
	// 中心座標
	Vector3 center = { 0,0,0 };

	// 半径
	Vector3 minRadius = { 1,1,1 };
	Vector3 maxRadius = { 1,1,1 };

	// 中心からの+と-のそれぞれの半径
	Vector3 minCenterRadius = { 0,0,0 };
	Vector3 maxCenterRadius = { 0,0,0 };
};

// OBB
struct OBB
{
	// 中心座標
	Vector3 centerPos = { 0,0,0 };
	// 方向ベクトル x,y,z
	Vector3 directionVector[3];
	// 半径
	Vector3 minRadius = { 1,1,1 };
	Vector3 maxRadius = { 1,1,1 };

	// 軸の長さ x,y,z
	float shaftLength[3];

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

