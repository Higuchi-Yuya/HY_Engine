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

Quaternion Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	float cos = dot(q1, q2);
	Quaternion t2 = q2;
	if (cos < 0.0f) {
		cos = -cos;
		t2 = { -q2.x,-q2.y,-q2.z, -q2.w };
	}
	float k0 = 1.0f - t;
	float k1 = t;

	return q1 * k0 + t2 * k1;
}

Quaternion Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2, float t)
{
	float cos = dot(q1, q2);
	Quaternion t2 = q2;
	if (cos < 0.0f) {
		cos = -cos;
		t2 = { -q2.x,-q2.y,-q2.z, -q2.w };
	}
	float k0 = 1.0f - t;
	float k1 = t;

	if ((1.0f - cos) > 0.001f) {
		float theta = (float)acos(cos);
		k0 = (float)(sin(theta * k0) / sin(theta));
		k1 = (float)(sin(theta * k1) / sin(theta));
	}

	// θ=0のときにも正しく球面線形補間が行われるように
	// EPSILONは極小の値で1.0fに近い値の時には線形補間を利用する。
	// 0.0005fとかそういう値でいいが、大きすぎて用いさすげても駄目である
	// 計算精度の問題で、1.0fに近い状態（特異点に近い）は計算結果の誤差も大きいためこのようにする
	float EPSILON = 0.0005f;
	if (cos >= 1.0f - EPSILON) {
		return k0 * q1 + k1 * t2;
	}

	return q1 * k0 + t2 * k1;
}

float Quaternion::dot(const Quaternion& q1, const Quaternion& q2)
{
	return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}

float Quaternion::length(const Quaternion& q)
{
	return (float)sqrt(dot(q, q));
}

Quaternion Quaternion::operator-(const Quaternion& q)
{
	Quaternion result = { -q.x,-q.y,-q.z,-q.w };
	return result;
}

Quaternion& Quaternion::operator+=(const Quaternion& q)
{
	Quaternion result = *this;
	result.x += q.x;
	result.y += q.y;
	result.z += q.z;
	result.w += q.w;
	return result;
}

Quaternion& Quaternion::operator-=(const Quaternion& q)
{
	Quaternion result = *this;
	result.x -= q.x;
	result.y -= q.y;
	result.z -= q.z;
	result.w -= q.w;
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

Quaternion& Quaternion::operator*=(float s)
{
	Quaternion result = *this;
	result.x *= s;
	result.y *= s;
	result.z *= s;
	result.w *= s;
	return result;
}

const Quaternion operator+(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion result = q1;
	return result += q2;
}

const Quaternion operator-(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion result = q1;
	return result -= q2;
}

const Quaternion operator*(const Quaternion& q1, const Quaternion& q2)
{
	Quaternion result = q1;
	return result *= q2;
}

const Quaternion operator*(const Quaternion& q, float s)
{
	Quaternion result = q;
	return result *= s;
}

const Quaternion operator*(float s, const Quaternion& q)
{
	Quaternion result = q;
	return result *= s;
}
