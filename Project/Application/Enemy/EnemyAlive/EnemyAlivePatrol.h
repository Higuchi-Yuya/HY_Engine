#pragma once
#include "IEnemyAliveState.h"

class EnemyAlivePatrol:public IEnemyAliveState
{
public:
	// 更新処理
	void UpdateAlive(Enemy& enemy, State::AliveState* aliveState)override;
};

