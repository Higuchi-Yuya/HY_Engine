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

	// Quaternionの積
	Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);

	// 単位Quaternionを返す
	Quaternion IdentityQuaternion();

	// 共役のQuaternionを返す
	Quaternion Conjugate(const Quaternion& quaternion);

	// Quaternionのnormを返す
	float Norm(const Quaternion& quaternion);

	// 正規化したQuaternionを返す
	Quaternion Normalize(const Quaternion& quaternion);

	// 逆Quaternionを返す
	Quaternion Inverse(const Quaternion& quaternion);

	// 任意軸回転を表すQuaternionの生成
	Quaternion MakeAxisAngle(const Vector3& axis, float angle);

	// ベクトルをQuaternionで回転させた結果のベクトルを求める
	Vector3 RotateVector(const Vector3& vector, const Quaternion& quaternion);

	// Quaternionから回転行列を求める
	Matrix4 MakeRotateMatrix(const Quaternion& quaternion);


	// 代入演算子オーバーロード
	Quaternion& operator/=(float s);
	Quaternion& operator*=(const Quaternion& q);
};

// 2項演算子オーバーロード
const Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
