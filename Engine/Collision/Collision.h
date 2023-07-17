#pragma once
#include "CollisionPrimitive.h"


class Collision
{
public:
	// ���ƕ��ʂ̓����蔻��
	static bool CheckSphere2Plane(const Sphere& sphere, const Plane& plane, Vector3* inter = nullptr);

	// ���Ƌ�
	static bool CheckSphere2Sphere(const Sphere& sphereA, const Sphere& sphereB, Vector3* inter = nullptr, Vector3* reject = nullptr);
	static bool CheckSphere2SphereOiOi(const Sphere& sphereA, const Sphere& sphereB, Vector3* inter = nullptr, Vector3* reject = nullptr);

	// �_�ƎO�p�`�̍ŋߐړ_�����߂�
	static void ClosestPtPoint2Triangle(const Vector3& point, const Triangle& triangle, Vector3* closest);

	// ���ƎO�p�`�̓�����`�F�b�N
	static bool CheckSphere2Triangle(const Sphere& sphere, const Triangle& triangle, Vector3* inter = nullptr, Vector3* reject = nullptr);

	// ���C�ƕ��ʂ̓����蔻��
	static bool CheckRay2Plane(const Ray& ray, const Plane& plane, float* distance = nullptr, Vector3* inter = nullptr);

	// ���C�Ɩ@���t���O�p�`�̓����蔻��
	static bool CheckRay2Triangle(const Ray& ray, const Triangle& triangle, float* distance = nullptr, Vector3* inter = nullptr);

	// ���C�Ƌ��̓����蔻��
	static bool CheckRay2Sphere(const Ray& ray, const Sphere& sphere, float* distance = nullptr, Vector3* inter = nullptr);


	static bool CheckAABB(const WorldTransform& transA, const WorldTransform& transB);
};

