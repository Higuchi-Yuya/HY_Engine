#pragma once
#pragma once
#include "Object3d.h"
#include <cassert>


class PlayerBullet:public Object3d
{
public:
	void Initialize(Model* model, const WorldTransform worldTransform, const Vector3& velocity);

	void Update()override;

	void Draw(ViewProjection* view)override;

	bool IsDead() const { return isDead_; }

	// ワールド弾座標を取得
	Vector3 GetBulletWorldPosition();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();
private:

	//速度
	Vector3 velocity_;

	//寿命<frm>
	static const int32_t kLifeTime = 60 * 5;

	//デスタイマー
	int32_t deathTimer_ = kLifeTime;

	//デスフラグ
	bool isDead_ = false;
};

