#pragma once
#include "CollisionTypes.h"
#include "Object3d.h"
#include "CollisionInfo.h"

/// <summary>
/// コライダー基底クラス
/// </summary>
class BaseCollider
{
public:
	friend class CollisionManager;
	BaseCollider() = default;
	// 仮想デストラクタ
	virtual ~BaseCollider() = default;

	inline void SetObject(Object3d* object) {this->object3d_ = object;}

	inline Object3d* GetObject3d() { return object3d_; }

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	// 形状タイプ取得
	inline CollisionShapeType GetShapeType() { return shapeType_; }

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	inline void OnCollision(const CollisionInfo& info) { object3d_->OnCollision(info); }

	/// <summary>
	/// 当たり判定属性をセット
	/// </summary>
	/// <param name="attribute">当たり判定属性</param>
	inline void SetAttribute(unsigned short attribute) {
		this->attribute_ = attribute;
	}

	/// <summary>
	/// 当たり判定属性を追加
	/// </summary>
	/// <param name="attribute">当たり判定属性</param>
	inline void AddAttribute(unsigned short attribute) {
		this->attribute_ |= attribute;
	}

	/// <summary>
	/// 当たり判定属性を削除
	/// </summary>
	/// <param name="attribute">当たり判定属性</param>
	inline void RemoveAttribute(unsigned short attribute) {
		this->attribute_ &= !attribute;
	}

protected:
	Object3d* object3d_ = nullptr;
	// 形状タイプ
	CollisionShapeType shapeType_ = SHAPE_UNKNOWN;
	// 当たり判定属性
	unsigned short attribute_ = 0b1111111111111111;
};

