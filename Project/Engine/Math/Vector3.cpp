#include"Vector3.h"
#include <cmath>


Vector3::Vector3()
	:x(0), y(0), z(0)
{
}

Vector3::Vector3(float x, float y, float z)
	: x(x), y(y), z(z)
{
}

float Vector3::length() const
{
	return std::sqrt(x * x + y * y + z * z);
}

float Vector3::length(Vector3 v1) const
{
	return std::sqrt(v1.x * v1.x + v1.y * v1.y + v1.z * v1.z);
}

Vector3& Vector3::normalize()
{
	float len = length();
	if (len != 0)
	{
		return *this /= length();
	}

	return *this;

}

float Vector3::dot(const Vector3& v, const Vector3& v2)
{
	return v.x * v2.x + v.y * v2.y + v.z * v2.z;
}

Vector3 Vector3::cross(const Vector3& v) const
{
	return Vector3(
		y * v.z - z * v.y,
		z * v.x - x * v.z,
		x * v.y - y * v.x
	);
}

Vector3 Vector3::cross(const Vector3& v1, const Vector3& v2) const
{
	return Vector3(
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	);
}

Vector3 Vector3::Cross(const Vector3 v1, const Vector3 v2)
{
	return Vector3(
		v1.y * v2.z - v1.z * v2.y,
		v1.z * v2.x - v1.x * v2.z,
		v1.x * v2.y - v1.y * v2.x
	);
}

Vector3 Vector3::distanceFrom(const Vector3& v, const Vector3& v2) const
{
	return  Vector3(
		(float)std::sqrt(std::pow(v2.x - v.x, 2)),
		(float)std::sqrt(std::pow(v2.y - v.y, 2)),
		(float)std::sqrt(std::pow(v2.z - v.z, 2))
	);
}

const Vector3 Vector3::lerp(const Vector3& start, const Vector3& end, const float t)
{
	return start * (1.0f - t) + end * t;
}

Vector3 Vector3::operator+() const
{
	return *this;
}

Vector3 Vector3::operator-() const
{
	return Vector3(-x, -y, -z);
}

Vector3& Vector3::operator+=(const Vector3& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

Vector3& Vector3::operator-=(const Vector3& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

Vector3& Vector3::operator*=(float s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3& Vector3::operator*=(const Vector3& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;
	return *this;
}

Vector3& Vector3::operator/=(float s)
{
	x /= s;
	y /= s;
	z /= s;
	return *this;
}

bool Vector3::operator!=(float num)
{
	return x != num || y != num || z != num;
}

const Vector3 operator+(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp += v2;
}

const Vector3 operator-(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);
	return temp -= v2;
}

const Vector3 operator*(const Vector3& v1, const Vector3& v2)
{
	Vector3 temp(v1);

	return temp *= v2;
}

const Vector3 operator*(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp *= s;
}

const Vector3 operator*(float s, const Vector3& v)
{
	return  v * s;
}

const Vector3 operator/(const Vector3& v, float s)
{
	Vector3 temp(v);
	return temp /= s;
}
