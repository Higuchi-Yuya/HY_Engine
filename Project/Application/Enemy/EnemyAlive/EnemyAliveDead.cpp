#include "EnemyAliveDead.h"
#include "Enemy.h"

void EnemyAliveDead::UpdateAlive(Enemy& enemy, State::AliveState* aliveState)
{
	aliveState;
	// ディゾルブの処理
	disoTimer_++;
	disoTimeLate_ = disoTimer_ / disoTimeMax_;
	enemy.dissolve_.isActiveDissolve_ = true;
	enemy.dissolve_.dissolveColor_ = Vector4(0.15f, 0.0f, 0.0f, 1);
	enemy.dissolve_.dissolveTime_ = disoTimeLate_;

	if (enemy.dissolve_.dissolveTime_ >= 1.0f) {
		IsDissolveEnd = true;
	}
}
