#include "Quaternion.h"
#include <cmath>
Quaternion::Quaternion()
{
	x = 0, y = 0, z = 0, w = 1;
}
// Quaternion�̐�
Quaternion Quaternion::Multiply(const Quaternion& lhs, const Quaternion& rhs)
{
	Quaternion result = 
	{
		lhs.y * rhs.z - lhs.z * rhs.y + rhs.w * lhs.x + lhs.w * rhs.x,  // x(i)
		lhs.z * rhs.x - lhs.x * rhs.z + rhs.w * lhs.y + lhs.w * rhs.y,  // y(j)
		lhs.x * rhs.y - lhs.y * rhs.x + rhs.w * lhs.z + lhs.w * rhs.z,  // z(k)
		lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z   // w
	};
	
	return result;
}

// �P��Quaternion��Ԃ�
Quaternion Quaternion::IdentityQuaternion()
{
	Quaternion result = { 0,0,0,1 };
	return result;
}

// ������Quaternion��Ԃ�
Quaternion Quaternion::Conjugate(const Quaternion& quaternion)
{
	Quaternion result = { -quaternion.x,-quaternion.y,-quaternion.z,quaternion.w };
	return result;
}

// Quaternion��norm��Ԃ�
float Quaternion::Norm(const Quaternion& quaternion)
{
	return std::sqrtf(quaternion.x * quaternion.x + 
		              quaternion.y * quaternion.y + 
		              quaternion.z * quaternion.z + 
		              quaternion.w * quaternion.w);
}

// ���K������Quaternion��Ԃ�
Quaternion Quaternion::Normalize(const Quaternion& quaternion)
{
	float len = Norm(quaternion);
	Quaternion result = quaternion;
	if (len != 0)
	{
		return result /= Norm(quaternion);
	}

	return result;
}

// �tQuaternion��Ԃ�
Quaternion Quaternion::Inverse(const Quaternion& quaternion)
{
	Quaternion result = quaternion;

	// ������Quaternion
	Quaternion conjugate = Conjugate(result);
	// �m������2��
	float norm = Norm(result) * Norm(result);
	
	conjugate /= norm;
	result = conjugate;

	return result;
}

Quaternion Quaternion::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;
	return *this;
}
