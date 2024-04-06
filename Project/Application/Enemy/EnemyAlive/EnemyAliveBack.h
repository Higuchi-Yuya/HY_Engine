#pragma once
#include "IEnemyAliveState.h"

class EnemyAliveBack:public IEnemyAliveState
{
public:
	// 更新処理
	void UpdateAlive(Enemy& enemy, State::AliveState* aliveState)override;
};

