#pragma once
#include "IEnemyAliveState.h"

class EnemyAliveDead:public IEnemyAliveState
{
public:
	// 更新処理
	void UpdateAlive(Enemy& enemy, State::AliveState* aliveState)override;
private:
	// ディゾルブ関連
	float disoTimer_ = 0;
	float disoTimeMax_ = 60 * 3;
	float disoTimeLate_ = 0;
};

