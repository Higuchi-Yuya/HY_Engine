#pragma once
#include "BaseCollider.h"
#include "Vector3.h"

class Object3d;

struct RaycastHit
{
	// �Փˑ���̃I�u�W�F�N�g
	Object3d* object = nullptr;
	// �Փˑ���̃R���C�_�[
	BaseCollider* collider = nullptr;
	// �Փ˓_
	Vector3 inter;
	// �Փ˓_�܂ł̋���
	float distance = 0.0f;

};

