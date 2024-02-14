#pragma once
#include "IEnemyState.h"

class EnemyAliveState:public IEnemyState
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
	void Update(Enemy& enemy)override;
private:
	/// <summary>
	/// 生きている時の更新処理
	/// </summary>
	void AliveUpdate(Enemy& enemy);

	/// <summary>
	/// 巡回の更新処理
	/// </summary>
	void PatrolUpdate(Enemy& enemy);

	/// <summary>
	/// 移動の更新処理
	/// </summary>
	void TrackingUpdate(Enemy& enemy);

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

