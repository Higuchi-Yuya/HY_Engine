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

	Quaternion operator/=(float s);
};

