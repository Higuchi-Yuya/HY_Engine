#pragma once
#include "CollisionTypes.h"
#include "Object3d.h"
#include "CollisionInfo.h"

/// <summary>
/// �R���C�_�[���N���X
/// </summary>
class BaseCollider
{
public:
	friend class CollisionManager;
	BaseCollider() = default;
	// ���z�f�X�g���N�^
	virtual ~BaseCollider() = default;

	inline void SetObject(Object3d* object) {this->object3d_ = object;}

	inline Object3d* GetObject3d() { return object3d_; }

	/// <summary>
	/// �X�V
	/// </summary>
	virtual void Update() = 0;

	// �`��^�C�v�擾
	inline CollisionShapeType GetShapeType() { return shapeType_; }

	/// <summary>
	/// �Փˎ��R�[���o�b�N�֐�
	/// </summary>
	inline void OnCollision(const CollisionInfo& info) { object3d_->OnCollision(info); }

	/// <summary>
	/// �����蔻�葮�����Z�b�g
	/// </summary>
	/// <param name="attribute">�����蔻�葮��</param>
	inline void SetAttribute(unsigned short attribute) {
		this->attribute_ = attribute;
	}

	/// <summary>
	/// �����蔻�葮����ǉ�
	/// </summary>
	/// <param name="attribute">�����蔻�葮��</param>
	inline void AddAttribute(unsigned short attribute) {
		this->attribute_ |= attribute;
	}

	/// <summary>
	/// �����蔻�葮�����폜
	/// </summary>
	/// <param name="attribute">�����蔻�葮��</param>
	inline void RemoveAttribute(unsigned short attribute) {
		this->attribute_ &= !attribute;
	}

protected:
	Object3d* object3d_ = nullptr;
	// �`��^�C�v
	CollisionShapeType shapeType_ = SHAPE_UNKNOWN;
	// �����蔻�葮��
	unsigned short attribute_ = 0b1111111111111111;
};

