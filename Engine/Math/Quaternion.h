#pragma once
#pragma once
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

	Quaternion operator/=(float s);
};

