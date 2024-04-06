#include "IEnemyAliveState.h"
#include "Player.h"
#include "Enemy.h"

Player* IEnemyAliveState::player_ = nullptr;

void IEnemyAliveState::Init(Enemy& enemy)
{
    testW_ = enemy.worldTransform_;
    testW_.translation.x += 5;

    patrolPos_.push_back(enemy.worldTransform_.translation);
    patrolPos_.push_back(testW_.translation);
}

bool IEnemyAliveState::GetIsDissolveEnd()
{
    return IsDissolveEnd;
}

void IEnemyAliveState::SetValue(Easing* easeP, Easing* easeA)
{
    easePatrol_ = easeP;
    easeAlpha_ = easeA;
}

void IEnemyAliveState::SetPlayer(Player* player)
{
	player_ = player;
}
