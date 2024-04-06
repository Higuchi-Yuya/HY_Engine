#pragma once
#include "IEnemyAliveState.h"
#include <memory>
#include "EnemyAlivePatrol.h"
#include "EnemyAliveBack.h"
#include "EnemyAliveTracking.h"
#include "EnemyAliveDead.h"

class Enemy;

class EnemyAliveState:public IEnemyState
{
public:

	// 更新処理
	void Update(Enemy& enemy)override;

private:
	void Init(Enemy& enemy);

	/// <summary>
	/// 生きている時の更新処理
	/// </summary>
	void AliveUpdate(Enemy& enemy);

	// 状態を変更
	bool ChangeStateCheck();



private:

	// 初期化フラグ
	bool IsInit_ = false;

	// 生きている時の状態
	IEnemyAliveState* cureentAliveState_;

	std::unique_ptr<EnemyAlivePatrol> aPatrol_;
	std::unique_ptr<EnemyAliveBack> aBack_;
	std::unique_ptr<EnemyAliveTracking> aTracking_;
	std::unique_ptr<EnemyAliveDead> aDead_;

};

