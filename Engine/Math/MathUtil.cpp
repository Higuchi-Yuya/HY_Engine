#include "MathUtil.h"

Matrix4 MathUtil::AssimpMatToMat4(aiMatrix4x4 mat4)
{
	Matrix4 assimp;

	assimp.m[0][0] = mat4.a1;
	assimp.m[0][1] = mat4.a2;
	assimp.m[0][2] = mat4.a3;
	assimp.m[0][3] = mat4.a4;

	assimp.m[1][0] = mat4.b1;
	assimp.m[1][1] = mat4.b2;
	assimp.m[1][2] = mat4.b3;
	assimp.m[1][3] = mat4.b4;

	assimp.m[2][0] = mat4.c1;
	assimp.m[2][1] = mat4.c2;
	assimp.m[2][2] = mat4.c3;
	assimp.m[2][3] = mat4.c4;

	assimp.m[3][0] = mat4.d1;
	assimp.m[3][1] = mat4.d2;
	assimp.m[3][2] = mat4.d3;
	assimp.m[3][3] = mat4.d4;

	return assimp;
}

Vector3 MathUtil::AssimpVec3ToVector3(aiVector3D assimpVec3)
{
	Vector3 vec3;

	vec3.x = assimpVec3.x;
	vec3.y = assimpVec3.y;
	vec3.z = assimpVec3.z;

	return vec3;
}

Vector4 MathUtil::AssimpQuaternionToVec4(aiQuaternion assimpaiQuaternion)
{
	Vector4 quaternion;

	quaternion.w = assimpaiQuaternion.w;
	quaternion.x = assimpaiQuaternion.x;
	quaternion.y = assimpaiQuaternion.y;
	quaternion.z = assimpaiQuaternion.z;

	return quaternion;
}

Quaternion MathUtil::AssimpQuaternion(aiQuaternion assimpaiQuaternion)
{
	Quaternion quaternion;

	quaternion.w = assimpaiQuaternion.w;
	quaternion.x = assimpaiQuaternion.x;
	quaternion.y = assimpaiQuaternion.y;
	quaternion.z = assimpaiQuaternion.z;

	return quaternion;
}

Vector4 MathUtil::AssimpQuaternionSlerp(aiQuaternion& assimpaiQuaternion, aiQuaternion& assimpaiQuaternion2, float t)
{
	Quaternion assimQ1 = AssimpQuaternion(assimpaiQuaternion);
	Quaternion assimQ2 = AssimpQuaternion(assimpaiQuaternion2);

	assimQ1.Slerp(assimQ2, assimQ1, t);

	Vector4 slrap = { assimQ1.x,assimQ1.y,assimQ1.z,assimQ1.w };

	return slrap;
}