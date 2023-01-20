#include "Quaternion.h"
#include <cmath>
Quaternion::Quaternion()
{
	x = 0, y = 0, z = 0, w = 1;
}
// Quaternionの積
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

// 単位Quaternionを返す
Quaternion Quaternion::IdentityQuaternion()
{
	Quaternion result = { 0,0,0,1 };
	return result;
}

// 共役のQuaternionを返す
Quaternion Quaternion::Conjugate(const Quaternion& quaternion)
{
	Quaternion result = { -quaternion.x,-quaternion.y,-quaternion.z,quaternion.w };
	return result;
}

// Quaternionのnormを返す
float Quaternion::Norm(const Quaternion& quaternion)
{
	return std::sqrtf(quaternion.x * quaternion.x + 
		              quaternion.y * quaternion.y + 
		              quaternion.z * quaternion.z + 
		              quaternion.w * quaternion.w);
}

// 正規化したQuaternionを返す
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

// 逆Quaternionを返す
Quaternion Quaternion::Inverse(const Quaternion& quaternion)
{
	Quaternion result = quaternion;

	// 共役のQuaternion
	Quaternion conjugate = Conjugate(result);
	// ノルムの2乗
	float norm = Norm(result) * Norm(result);
	
	conjugate /= norm;
	result = conjugate;

	return result;
}

// 任意軸回転を表すQuaternionの生成
Quaternion Quaternion::MakeAxisAngle(const Vector3& axis, float angle)
{
	float _sin = sin(angle / 2.0f);
	return Quaternion(_sin * axis.x, _sin * axis.y, _sin * axis.z, cos(angle / 2.0f));

}

// ベクトルをQuaternionで回転させた結果のベクトルを求める
Vector3 Quaternion::RotateVector(const Vector3& vector, const Quaternion& quaternion)
{
	// 3次元ベクトルをQuaternionで表す
	Quaternion vec3Quaternion = { vector.x,vector.y ,vector.z ,0 };

	// 単位Quaternion
	Quaternion identityQuaternion = Conjugate(quaternion);

	Quaternion resultQuaternion = quaternion * vec3Quaternion * identityQuaternion;

	return Vector3(resultQuaternion.x, resultQuaternion.y, resultQuaternion.z);
}

// Quaternionから回転行列を求める
Matrix4 Quaternion::MakeRotateMatrix(const Quaternion& quaternion)
{
	float xx = quaternion.x * quaternion.x;
	float yy = quaternion.y * quaternion.y;
	float zz = quaternion.z * quaternion.z;
	float ww = quaternion.w * quaternion.w;
	float xy = quaternion.x * quaternion.y * 2.0f;
	float xz = quaternion.x * quaternion.z * 2.0f;
	float yz = quaternion.y * quaternion.z * 2.0f;
	float wx = quaternion.w * quaternion.x * 2.0f;
	float wy = quaternion.w * quaternion.y * 2.0f;
	float wz = quaternion.w * quaternion.z * 2.0f;

	Matrix4 result{
		ww + xx - yy - zz, xy + wz          , xz - wy          ,0.0f,
		xy - wz          , ww - xx + yy - zz, yz + wx          ,0.0f,
		xz + wy          , yz - wx          , ww - xx - yy + zz,0.0f,
		0.0f             ,0.0f              ,0.0f              ,1.0f
	};


	return result;
}

Quaternion& Quaternion::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	w /= s;
	return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& q)
{
	Quaternion result = Multiply(*this, q);
	return result;
}

const Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion result = q1;
	return result *= q2;
}
