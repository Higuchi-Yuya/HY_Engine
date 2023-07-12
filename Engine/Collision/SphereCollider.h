#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"

#include "Vector3.h"
#include "Matrix4.h"


class SphereCollider:public BaseCollider,public Sphere
{
private:// �G�C���A�X
public:
	SphereCollider(Vector3 offset = { 0,0,0 }, float radius = 1.0f) :offset_(offset), radius_(radius) {
		// ���`����Z�b�g
		shapeType_ = COLLISIONSHAPE_SPHERE;
	}

	// �X�V
	void Update(const Matrix4& worldPos)override;
	
	inline const Vector3& GetOffset() { return offset_; }

	inline void SetOffset(const Vector3& offset) { this->offset_ = offset; }

	inline float GetRadius() { return radius_; }

	inline void SetRadius(float radius) {radius_ = radius; }

private:
	// �I�u�W�F�N�g���S����̃I�t�Z�b�g
	Vector3 offset_;
	// ���a
	float radius_;
};

