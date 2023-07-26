#pragma once
#include "Vector3.h"
class Object3d;
class BaseCollider;

/// <summary>
/// クエリーによる情報を得るための構造体
/// </summary>
struct QueryHit
{
	// 衝突相手のオブジェクト
	Object3d* object = nullptr;
	// 衝突相手のコライダー
	BaseCollider* collider = nullptr;
	// 衝突点
	Vector3 inter;
	// 排斥ベクトル
	Vector3 reject;

};

/// <summary>
/// クエリーで交差を検出した時の動作を規定するクラス
/// </summary>
class QueryCallback
{
public:
	QueryCallback() = default;
	virtual ~QueryCallback() = default;

	// 交差時コールバック
	virtual bool OnQueryHit(const QueryHit& info) = 0;
};

