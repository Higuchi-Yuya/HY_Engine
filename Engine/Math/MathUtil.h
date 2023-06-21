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


};

