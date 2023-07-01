#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include <DirectXMath.h>
#include<assimp/scene.h>

// ���w�֗��֐��܂Ƃ�
namespace MathUtil
{
	// �A�V���v�̃}�g���b�N�X������̂��̂ɕϊ�
	Matrix4 AssimpMatToMat4(aiMatrix4x4 mat4);

	// �A�V���v�̃x�N�^�[�R�������̃x�N�^�[�R�ɕϊ�
	Vector3 AssimpVec3ToVector3(aiVector3D assimpVec3);

	// �A�V���v��Quaternion��Vector4�ɕϊ�
	Vector4 AssimpQuaternionToVec4(aiQuaternion assimpaiQuaternion);

	// �A�V���v��Quaternion��������Quaternion�ɕϊ�
	Quaternion AssimpQuaternion(aiQuaternion assimpaiQuaternion);

	// �A�V���v��Quaternion��Slerp����
	Vector4 AssimpQuaternionSlerp(aiQuaternion& assimpaiQuaternion, aiQuaternion& assimpaiQuaternion2, float t);

	//�x�N�g���ƍs��̊|���Z(�o��Vector3)
	Vector3 MatVector(Matrix4 matrix4, Vector3 vector3);

	// �x�����烉�W�A��
	float DegreeToRadian(float degree);

	Vector3 DegreeToRadianVec3(Vector3 degree);

	// sin�g�̓��������֐�
	float Sin_ZeroToOne(float pos, float maxCount, float nowCount, float swingWidth);

	// �l��Min��Max�̊Ԃɐ�������֐�
	template<typename T>
	T Clamp(const T value, const T min = 0, const T max = 1)
	{
		if (value < min)
		{
			return min;
		}
		if (value > max)
		{
			return max;
		}
		return value;
	}
};

