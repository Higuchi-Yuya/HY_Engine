#include "EnemyAliveBack.h"
#include "Enemy.h"

void EnemyAliveBack::UpdateAlive(Enemy& enemy, State::AliveState* aliveState)
{
	// アルファ値がゼロではないとき
	if (IsAlphaZero_ == false) {
		easeAlpha_->SetEaseLimitTime(backWaitTimeLimit_);

		easeAlpha_->Update();
		enemy.worldTransform_.color.w = easeAlpha_->easeOutCubic(1, 0);

		if (easeAlpha_->GetIsEnd() == true) {
			easeAlpha_->Reset();
			IsAlphaZero_ = true;
			enemy.worldTransform_.translation.x = patrolPos_[0].x;
			enemy.worldTransform_.translation.z = patrolPos_[0].z;
			enemy.worldTransform_.scale = { 1,1,1 };
		}
	}
	// アルファ値がぜろのとき
	if (IsAlphaZero_ == true) {
		easeAlpha_->SetEaseLimitTime(backWaitTimeLimit_);

		easeAlpha_->Update();
		enemy.worldTransform_.color.w = easeAlpha_->easeOutCubic(0, 1);

		if (easeAlpha_->GetIsEnd() == true) {
			easeAlpha_->Reset();
			*aliveState = State::AliveState::Patrol;
		}
	}
}
