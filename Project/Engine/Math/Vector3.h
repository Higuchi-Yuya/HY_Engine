#pragma once
#include <assimp/vector3.h>

class Vector3
{
public:
	float x;
	float y;
	float z;
public:
	//�R���X�g���N�^
	Vector3();
	Vector3(float x, float y, float z);

	//�����o�֐�
	float length()const;
	float length(Vector3 v1)const;

	Vector3& normalize();
	static float dot(const Vector3& v, const Vector3& v2);
	Vector3 cross(const Vector3& v)const;
	Vector3 cross(const Vector3& v1, const Vector3& v2)const;
	static Vector3 Cross(const Vector3 v1, const Vector3 v2);		// ��̃x�N�g���̊O��
	Vector3 distanceFrom(const Vector3& v, const Vector3& v2)const;

	// ��Ԋ֐�
	// ���`���(1���֐����)
	static const Vector3 lerp(const Vector3& start, const Vector3& end, const float t);

	//�x���@���ʓx�@�ɕϊ�����֐�
	static float Deg2Rad(float Deg) {
		float PI = 3.141592f;
		return Deg * PI / 180;

	}

	//�ʓx�@��x���@�ɕϊ�����֐�
	static float Rad2Deg(float rad) {
		float PI = 3.141592f;
		return rad / PI * 180;

	}

	//�P�����Z�q�I�[�o�[���[�h
	Vector3 operator+()const;
	Vector3 operator-()const;

	//������Z�q�I�[�o�[���[�h
	Vector3& operator+=(const Vector3& v);
	Vector3& operator-=(const Vector3& v);
	Vector3& operator*=(float s);
	Vector3& operator*=(const Vector3& v);
	Vector3& operator/=(float s);

	// ��r���Z�q
	bool operator!=(float num);
};


//2�����Z�q�I�[�o�[���[�h
//�������Ȉ����̃p�^�[���ɑΉ�(�����̏���)���邽�߁A�ȉ��̂悤�ɏ������Ă���
const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator/(const Vector3& v, float s);




