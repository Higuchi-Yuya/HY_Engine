#pragma once
#include "Vector3.h"
class Object3d;
class BaseCollider;

/// <summary>
/// �N�G���[�ɂ����𓾂邽�߂̍\����
/// </summary>
struct QueryHit
{
	// �Փˑ���̃I�u�W�F�N�g
	Object3d* object = nullptr;
	// �Փˑ���̃R���C�_�[
	BaseCollider* collider = nullptr;
	// �Փ˓_
	Vector3 inter;
	// �r�˃x�N�g��
	Vector3 reject;

};

/// <summary>
/// �N�G���[�Ō��������o�������̓�����K�肷��N���X
/// </summary>
class QueryCallback
{
public:
	QueryCallback() = default;
	virtual ~QueryCallback() = default;

	// �������R�[���o�b�N
	virtual bool OnQueryHit(const QueryHit& info) = 0;
};

