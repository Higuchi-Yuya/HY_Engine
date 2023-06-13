#include "SphereCollider.h"

void SphereCollider::Update()
{
	// ワールド行列から座標を抽出
	const Matrix4& matWorld = object3d_->GetMatWorld();

	const Vector3& position = { matWorld.m[3][0],matWorld.m[3][1] ,matWorld.m[3][2] };

	// 球のメンバ変数を更新
	Sphere::center = position + offset_;
	Sphere::radius = radius;
}
