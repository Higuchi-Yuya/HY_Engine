#pragma once
#include "IEnemyAliveState.h"

class EnemyAliveTracking:public IEnemyAliveState
{
private:
	// プレイヤーを狙うときの挙動状態
	enum TarkingState
	{
		ScaleBig,
		ScaleSmall,
	};

public:
	// 更新処理
	void UpdateAlive(Enemy& enemy, State::AliveState* aliveState)override;

private:
	// 移動する前の古いポジション
	Vector3 oldPos_{};

	// 追従関連
	float followTimer = 0;
	float followTimeMax = 20;
	Vector3 followVec;
	float followSpeed = 0.1f;

	float timer = 0;
	float maxTime = 240;

	// スケールの変化関連
	TarkingState tState_ = ScaleBig;
	Easing easeScale_;
	Vector3 bigScale_ = { 1,1,1 };
	Vector3 smallScale_ = { 0.6f,0.6f,0.6f };
};

