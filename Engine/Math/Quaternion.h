#pragma once
#include "Vector3.h"
#include "Matrix4.h"

class Quaternion
{
public:
	float x;
	float y;
	float z;
	float w;

public:
	Quaternion();
	Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	// Quaternion�̐�
	Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

	// �P��Quaternion��Ԃ�
	Quaternion IdentityQuaternion();

	// ������Quaternion��Ԃ�
	Quaternion Conjugate(const Quaternion& quaternion);

	// Quaternion��norm��Ԃ�
	float Norm(const Quaternion& quaternion);

	// ���K������Quaternion��Ԃ�
	Quaternion Normalize(const Quaternion& quaternion);

	// �tQuaternion��Ԃ�
	Quaternion Inverse(const Quaternion& quaternion);

	// �C�ӎ���]��\��Quaternion�̐���
	Quaternion MakeAxisAngle(const Vector3& axis, float angle);

	// �x�N�g����Quaternion�ŉ�]���������ʂ̃x�N�g�������߂�
	Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

	// Quaternion�����]�s������߂�
	Matrix4 MakeRotateMatrix(const Quaternion& quaternion);


	// ������Z�q�I�[�o�[���[�h
	Quaternion& operator/=(float s);
	Quaternion& operator*=(const Quaternion& q);
};

// 2�����Z�q�I�[�o�[���[�h
const Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
