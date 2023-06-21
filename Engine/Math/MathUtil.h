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


};

