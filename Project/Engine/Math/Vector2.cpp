#include "Vector2.h"
#include<cmath>  //sprt


Vector2::Vector2() :x(0), y(0)
{
}

Vector2::Vector2(float x, float y){
	this->x = x;
	this->y = y;
}

float Vector2::length()const {
	return std::sqrt(x * x + y * y);
}

Vector2& Vector2::normalize() {
	float len = length();
	if (len != 0) {
		return *this /= len;
	}
	return *this;
}

float Vector2::dot(const Vector2& v) const
{
	return x * v.x + y * v.y;
}

float Vector2::cross(const Vector2& v) const
{
	return x * v.y - y * v.x;
}

Vector2& Vector2::operator+=(const Vector2& v)
{
	this->x += v.x;
	this->y += v.y;
	return *this;
}

Vector2& Vector2::operator-=(const Vector2& v)
{
	this->x -= v.x;
	this->y -= v.y;
	return *this;
}

Vector2& Vector2::operator*=(float s)
{
	this->x *= s;
	this->y *= s;
	return *this;
}

Vector2& Vector2::operator/=(float s)
{
	x /= s;
	y /= s;
	return *this;
}

bool Vector2::operator==(const Vector2 other)
{
	return x == other.x && y == other.y;
}

bool Vector2::operator!=(const Vector2 other)
{
	return x != other.x || y != other.y;
}

bool Vector2::operator>=(const Vector2 other)
{
	return x >= other.x && y >= other.y;
}

bool Vector2::operator<=(const Vector2 other)
{
	return x <= other.x && y <= other.y;
}

bool Vector2::operator==(float num)
{
	return x == num && y == num;
}

bool Vector2::operator!=(float num)
{
	return x != num || y != num;
}

bool Vector2::operator>=(float num)
{
	return x >= num && y >= num;
}

bool Vector2::operator<=(float num)
{
	return x <= num && y <= num;
}

//Vector2 クラスに属さない関数群
const Vector2 operator+(const Vector2& v1, const Vector2& v2)
{
	Vector2 temp(v1);
	return temp += v2;
}

const Vector2 operator-(const Vector2& v1, const Vector2& v2)
{
	Vector2 temp(v1);
	return temp -= v2;
}

const Vector2 operator*(const Vector2& v, float s)
{
	Vector2 temp(v);
	return temp *= s;
}

const Vector2 operator*(float s, const Vector2& v)
{
	return v * s;
}

const Vector2 operator/(const Vector2& v, float s)
{
	Vector2 temp(v);
	return temp /= s;
}