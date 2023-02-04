#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include "Vector3.h"
#include "Matrix4.h"


class SphereCollider:public BaseCollider,public Sphere
{
private:// エイリアス
public:
	SphereCollider(Vector3 offset = { 0,0,0 }, float radius = 1.0f) :offset(offset), radius(radius) {
		// 球形状をセット
		shapeType = COLLISIONSHAPE_SPHERE;
	}

	// 更新
	void Update()override;

	inline void SetRadius(float radius) { this->radius = radius; }

private:
	// オブジェクト中心からのオフセット
	Vector3 offset;
	// 半径
	float radius;
};

