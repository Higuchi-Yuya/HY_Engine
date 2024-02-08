#include "EnemyAliveState.h"
#include "Enemy.h"
void EnemyAliveState::Update(Enemy& enemy)
{
	AliveUpdate(enemy);
}

void EnemyAliveState::AliveUpdate(Enemy& enemy)
{
	switch (aState_)
	{
	case AliveState::Patrol:// 巡回
		PatrolUpdate(enemy);

		break;
	case AliveState::Back:// 戻るとき
		// アルファ値がゼロではないとき
		if (IsAlphaZero_ == false) {
			easeAlpha_.SetEaseLimitTime(backWaitTimeLimit_);

			easeAlpha_.Update();
			enemy.worldTransform_.color.w = easeAlpha_.easeOutCubic(1, 0);

			if (easeAlpha_.GetIsEnd() == true) {
				easeAlpha_.Reset();
				IsAlphaZero_ = true;
				enemy.worldTransform_.translation.x = patrolPos_[0].x;
				enemy.worldTransform_.translation.z = patrolPos_[0].z;
			}
		}
		// アルファ値がぜろのとき
		if (IsAlphaZero_ == true) {
			easeAlpha_.SetEaseLimitTime(backWaitTimeLimit_);

			easeAlpha_.Update();
			enemy.worldTransform_.color.w = easeAlpha_.easeOutCubic(0, 1);

			if (easeAlpha_.GetIsEnd() == true) {
				easeAlpha_.Reset();
				aState_ = AliveState::Patrol;
			}
		}
		break;
	case AliveState::Tracking:// 追跡
		TrackingUpdate(enemy);

		break;
	default:
		break;
	}

	// 死んだら状態を変更
	if (IsAlive_ == false) {
	}
}

void EnemyAliveState::PatrolUpdate(Enemy& enemy)
{
	int32_t patrolMaxNum = (int32_t)patrolPos_.size() - 1;

	// 行きルート
	if (IsPatrolEnd_ == false) {

		if (patrolPosNum_ != patrolMaxNum) {
			easePatrol_.SetEaseLimitTime(120);
			easePatrol_.Update();

			enemy.worldTransform_.translation.x = easePatrol_.Lerp(patrolPos_[patrolPosNum_].x, patrolPos_[patrolPosNum_ + 1].x);
			enemy.worldTransform_.translation.z = easePatrol_.Lerp(patrolPos_[patrolPosNum_].z, patrolPos_[patrolPosNum_ + 1].z);

			if (easePatrol_.GetIsEnd() == true) {

				patrolPosNum_++;
			}

		}
		if (patrolPosNum_ == patrolMaxNum &&
			easePatrol_.GetIsEnd() == true) {
			easePatrol_.Reset();
			IsPatrolEnd_ = true;
		}


	}
	// 戻りルート
	if (IsPatrolEnd_ == true) {

		if (patrolPosNum_ != 0) {
			easePatrol_.SetEaseLimitTime(120);
			easePatrol_.Update();

			enemy.worldTransform_.translation.x = easePatrol_.Lerp(patrolPos_[patrolPosNum_].x, patrolPos_[patrolPosNum_ - 1].x);
			enemy.worldTransform_.translation.z = easePatrol_.Lerp(patrolPos_[patrolPosNum_].z, patrolPos_[patrolPosNum_ - 1].z);

			if (easePatrol_.GetIsEnd() == true) {

				patrolPosNum_--;
			}

		}
		if (patrolPosNum_ == 0 &&
			easePatrol_.GetIsEnd() == true) {
			easePatrol_.Reset();
			IsPatrolEnd_ = false;
		}
	}

	// 回転処理
	enemy.worldTransform_.rotation.y += MathUtil::DegreeToRadian(5);
}

void EnemyAliveState::TrackingUpdate(Enemy& enemy)
{
	// 移動前ポジションを代入
	oldPos_ = enemy.worldTransform_.translation;

	timer++;
	followTimer++;
	if (timer > maxTime) {
		timer = 0;
	}
	// 借りてきたプレイヤーのポジション
	Vector3 playerPos = player_->worldTransform_.translation;

	if (followTimer > followTimeMax) {
		// 追従のベクトルの更新
		Vector3 velo = playerPos - enemy.worldTransform_.translation;
		followVec = velo.normalize() * followSpeed;
		followTimer = 0;
	}

	enemy.worldTransform_.translation += followVec;
	enemy.worldTransform_.translation.y = 1.0f;
	enemy.worldTransform_.rotation.y += MathUtil::DegreeToRadian(5);
}
