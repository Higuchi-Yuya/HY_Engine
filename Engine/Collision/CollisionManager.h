#pragma once
#include <forward_list>
#include "CollisionPrimitive.h"
#include "RaycastHit.h"

#include <d3d12.h>
class BaseCollider;

class CollisionManager
{
public:// 静的メンバ関数
	static CollisionManager* GetInstance();

public:// メンバ関数
	// コライダーの追加
	inline void AddCollider(BaseCollider* collider) {
		colliders.push_front(collider);
	}

	// コライダーの削除
	inline void RemoveCollider(BaseCollider* collider) {
		colliders.remove(collider);
	}


	// レイが任意のコライダーと交わる場合はtrue、それ以外はfalse
	bool Raycast(const Ray& ray, RaycastHit* hitInfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);


	// 全ての衝突チェック
	void CheckAllCollisions();

private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;

	// コライダーのリスト
	std::forward_list<BaseCollider*>colliders;
};

