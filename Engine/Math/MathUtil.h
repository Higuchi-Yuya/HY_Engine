#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include "Quaternion.h"
#include <DirectXMath.h>
#include<assimp/scene.h>

// 数学便利関数まとめ
namespace MathUtil
{
	// アシンプのマトリックスを自作のものに変換
	Matrix4 AssimpMatToMat4(aiMatrix4x4 mat4);

	// アシンプのベクター３を自分のベクター３に変換
	Vector3 AssimpVec3ToVector3(aiVector3D assimpVec3);

	// アシンプのQuaternionをVector4に変換
	Vector4 AssimpQuaternionToVec4(aiQuaternion assimpaiQuaternion);

	// アシンプのQuaternionを自分のQuaternionに変換
	Quaternion AssimpQuaternion(aiQuaternion assimpaiQuaternion);

	// アシンプのQuaternionをSlerpする
	Vector4 AssimpQuaternionSlerp(aiQuaternion& assimpaiQuaternion, aiQuaternion& assimpaiQuaternion2, float t);

	//ベクトルと行列の掛け算(出力Vector3)
	Vector3 MatVector(Matrix4 matrix4, Vector3 vector3);

	// 度数からラジアン
	float DegreeToRadian(float degree);

	Vector3 DegreeToRadianVec3(Vector3 degree);

	// sin波の動きを作る関数
	float Sin_ZeroToOne(float pos, float maxCount, float nowCount, float swingWidth);

	// 値をMinとMaxの間に制限する関数
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

