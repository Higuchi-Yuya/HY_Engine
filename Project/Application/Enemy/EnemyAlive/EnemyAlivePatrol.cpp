#include "EnemyAlivePatrol.h"
#include "Enemy.h"

void EnemyAlivePatrol::UpdateAlive(Enemy& enemy, State::AliveState* aliveState)
{
	int32_t patrolMaxNum = (int32_t)patrolPos_.size() - 1;
	aliveState;
	// 行きルート
	if (IsPatrolEnd_ == false) {

		if (patrolPosNum_ != patrolMaxNum) {
			easePatrol_->SetEaseLimitTime(120);
			easePatrol_->Update();

			enemy.worldTransform_.translation.x = easePatrol_->Lerp(patrolPos_[patrolPosNum_].x, patrolPos_[patrolPosNum_ + 1].x);
			enemy.worldTransform_.translation.z = easePatrol_->Lerp(patrolPos_[patrolPosNum_].z, patrolPos_[patrolPosNum_ + 1].z);

			if (easePatrol_->GetIsEnd() == true) {

				patrolPosNum_++;
			}

		}
		if (patrolPosNum_ == patrolMaxNum &&
			easePatrol_->GetIsEnd() == true) {
			easePatrol_->Reset();
			IsPatrolEnd_ = true;
		}


	}
	// 戻りルート
	if (IsPatrolEnd_ == true) {

		if (patrolPosNum_ != 0) {
			easePatrol_->SetEaseLimitTime(120);
			easePatrol_->Update();

			enemy.worldTransform_.translation.x = easePatrol_->Lerp(patrolPos_[patrolPosNum_].x, patrolPos_[patrolPosNum_ - 1].x);
			enemy.worldTransform_.translation.z = easePatrol_->Lerp(patrolPos_[patrolPosNum_].z, patrolPos_[patrolPosNum_ - 1].z);

			if (easePatrol_->GetIsEnd() == true) {

				patrolPosNum_--;
			}

		}
		if (patrolPosNum_ == 0 &&
			easePatrol_->GetIsEnd() == true) {
			easePatrol_->Reset();
			IsPatrolEnd_ = false;
		}
	}

	// 回転処理
	enemy.worldTransform_.rotation.y += MathUtil::DegreeToRadian(5);
}
