#pragma once
#include <forward_list>
#include "CollisionPrimitive.h"
#include "RaycastHit.h"

#include <d3d12.h>
class BaseCollider;

class CollisionManager
{
public:// �ÓI�����o�֐�
	static CollisionManager* GetInstance();

public:// �����o�֐�
	// �R���C�_�[�̒ǉ�
	inline void AddCollider(BaseCollider* collider) {
		colliders.push_front(collider);
	}

	// �R���C�_�[�̍폜
	inline void RemoveCollider(BaseCollider* collider) {
		colliders.remove(collider);
	}


	// ���C���C�ӂ̃R���C�_�[�ƌ����ꍇ��true�A����ȊO��false
	bool Raycast(const Ray& ray, RaycastHit* hitInfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);


	// �S�Ă̏Փ˃`�F�b�N
	void CheckAllCollisions();

private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;

	// �R���C�_�[�̃��X�g
	std::forward_list<BaseCollider*>colliders;
};

