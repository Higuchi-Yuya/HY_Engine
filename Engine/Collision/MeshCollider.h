#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include "Vector3.h"

/// <summary>
/// メッシュ衝突判定オブジェクト
/// </summary>
class MeshCollider:public BaseCollider
{
public:
	MeshCollider() {
		// メッシュ形状をセット
		shapeType_ = COLLISIONSHAPE_MESH;
	}

	/// <summary>
	/// 三角形の配列を構築する
	/// </summary>
	void ConstructTriangles(Model* model);

	// 更新
	void Update()override;

	// 球との当たり判定
	bool CheckCollisionSphere(const Sphere& sphere, Vector3* inter = nullptr, Vector3* reject = nullptr);

	// レイとの当たり判定
	bool CheckCollisionRay(const Ray& ray, float* distance = nullptr, Vector3* inter = nullptr);

private:
	std::vector<Triangle>triangles;
	// ワールド行列の逆行列
	Matrix4 invMatWorld;
};

