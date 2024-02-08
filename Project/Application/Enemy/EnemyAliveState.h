#pragma once
#include "IEnemyState.h"

class EnemyAliveState:public IEnemyState
{
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
};

