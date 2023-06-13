#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include "Vector3.h"

/// <summary>
/// ���b�V���Փ˔���I�u�W�F�N�g
/// </summary>
class MeshCollider:public BaseCollider
{
public:
	MeshCollider() {
		// ���b�V���`����Z�b�g
		shapeType_ = COLLISIONSHAPE_MESH;
	}

	/// <summary>
	/// �O�p�`�̔z����\�z����
	/// </summary>
	void ConstructTriangles(Model* model);

	// �X�V
	void Update()override;

	// ���Ƃ̓����蔻��
	bool CheckCollisionSphere(const Sphere& sphere, Vector3* inter = nullptr, Vector3* reject = nullptr);

	// ���C�Ƃ̓����蔻��
	bool CheckCollisionRay(const Ray& ray, float* distance = nullptr, Vector3* inter = nullptr);

private:
	std::vector<Triangle>triangles;
	// ���[���h�s��̋t�s��
	Matrix4 invMatWorld;
};

