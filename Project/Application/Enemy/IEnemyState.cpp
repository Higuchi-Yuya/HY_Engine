#include "IEnemyState.h"
#include "Enemy.h"
#include "Player.h"

Player* IEnemyState::player_ = nullptr;

IEnemyState::AliveState IEnemyState::GetAliveState()
{
    return aState_;
}

float IEnemyState::GetSpawnTimer()
{
    return spawnTimer;
}

float IEnemyState::GetSpawnTimeMax()
{
    return spawnTimeMax;
}

Vector3 IEnemyState::GetSpawnFirstPos()
{
    return spawnFirstPos_;
}

bool IEnemyState::GetIsAlive()
{
    return IsAlive_;
}

int32_t IEnemyState::GetPatrolPosNum()
{
    return patrolPosNum_;
}

Easing IEnemyState::GetPatrolEase()
{
    return easePatrol_;
}

bool IEnemyState::GetIsPatrolEnd()
{
    return IsPatrolEnd_;
}

Easing IEnemyState::GetAlphaEase()
{
    return easeAlpha_;
}

bool IEnemyState::GetIsAlphaZero()
{
    return IsAlphaZero_;
}

int IEnemyState::GetNearParticleNum()
{
    return nearPlayerParticleNum;
}

int IEnemyState::GetNearTimer()
{
    return nearPlayerTimer;
}

int IEnemyState::GetNearTimeLimit()
{
    return nearPlayerTimeLimit;
}

Vector4 IEnemyState::GetNearStartColor()
{
    return nearStartColor_;
}

Vector4 IEnemyState::GetNearEndColor()
{
    return nearEndColor_;
}

void IEnemyState::PatrolInit(Enemy& enemy)
{
    testW_ = enemy.worldTransform_;
    testW_.translation.x += 5;

    patrolPos_.push_back(enemy.worldTransform_.translation);
    patrolPos_.push_back(testW_.translation);
}

void IEnemyState::EaseReset()
{
    easeAlpha_.Reset();
    easePatrol_.Reset();
}

void IEnemyState::SetAliveState(const AliveState aliveState)
{
    aState_ = aliveState;
}

void IEnemyState::SetNearTimer(int nearTimer)
{
    nearPlayerTimer = nearTimer;
}

void IEnemyState::IncrementNearTimer()
{
    nearPlayerTimer++;
}

void IEnemyState::SetIsAlphaZero(bool IsAlphaZero)
{
    IsAlphaZero_ = IsAlphaZero;
}

void IEnemyState::SetIsPatrolEnd(bool IsPatrolEnd)
{
    IsPatrolEnd_ = IsPatrolEnd;
}

void IEnemyState::SetPlayer(Player* player)
{
    player_ = player;
}

void IEnemyState::SetPatrolNum(int32_t patrolNum)
{
    patrolPosNum_ = patrolNum;
}
