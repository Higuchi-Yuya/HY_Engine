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

	// Quaternion‚ÌÏ
	Quaternion Multiply(const Quaternion& lhs, const Quaternion& rhs);
	// ’PˆÊQuaternion‚ğ•Ô‚·
	Quaternion IdentityQuaternion();
	// ‹¤–ğ‚ÌQuaternion‚ğ•Ô‚·
	Quaternion Conjugate(const Quaternion& quaternion);
	// Quaternion‚Ìnorm‚ğ•Ô‚·
	float Norm(const Quaternion& quaternion);
	// ³‹K‰»‚µ‚½Quaternion‚ğ•Ô‚·
	Quaternion Normalize(const Quaternion& quaternion);
	// ‹tQuaternion‚ğ•Ô‚·
	Quaternion Inverse(const Quaternion& quaternion);

	Quaternion operator/=(float s);
};

