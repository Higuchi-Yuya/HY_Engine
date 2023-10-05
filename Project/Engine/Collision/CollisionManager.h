#pragma once

#include <forward_list>
#include "CollisionPrimitive.h"
#include "RaycastHit.h"
#include "QueryCallback.h"

#include <d3d12.h>
class BaseCollider;

class CollisionManager
{
public:// 静的メンバ関数
	static CollisionManager* GetInstance();

public:// メンバ関数
	// コライダーの追加
	inline void AddCollider(BaseCollider* collider) {
		colliders_.push_front(collider);
	}

	// コライダーの削除
	inline void RemoveCollider(BaseCollider* collider) {
		colliders_.remove(collider);
	}


	// レイが任意のコライダーと交わる場合はtrue、それ以外はfalse
	bool Raycast(const Ray& ray, RaycastHit* hitInfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);


	/// <summary>
	/// レイキャスト
	/// </summary>
	/// <param name="ray">レイ</param>
	/// <param name="hitInfo">対象の衝突属性</param>
	/// <param name="hitInfo">衝突情報</param>
	/// <param name="maxDistance">最大距離</param>
	/// <returns>レイが任意のコライダーと交わる場合はtrue、それ以外はfalse</returns>
	bool Raycast(const Ray& ray, unsigned short attribute, RaycastHit* hitInfo = nullptr, float maxDistance = D3D12_FLOAT32_MAX);

	// 球による衝突全検索
	void QuerySphere(const Sphere& sphere, QueryCallback* callback, unsigned short attribute = (unsigned short)0xff);

	// 全ての衝突チェック
	void CheckAllCollisions();

private:
	CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	~CollisionManager() = default;
	CollisionManager& operator=(const CollisionManager&) = delete;

	// コライダーのリスト
	std::forward_list<BaseCollider*>colliders_;
};

