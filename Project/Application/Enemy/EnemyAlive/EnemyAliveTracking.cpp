#include "EnemyAliveTracking.h"
#include "Enemy.h"

void EnemyAliveTracking::UpdateAlive(Enemy& enemy, State::AliveState* aliveState)
{
	// 移動前ポジションを代入
	oldPos_ = enemy.worldTransform_.translation;
	aliveState;
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

	// スケールの変化
	switch (tState_)
	{
	case ScaleBig:
		easeScale_.SetEaseLimitTime(30);
		easeScale_.Update();

		// スケール変化（大きいから小さい）
		enemy.worldTransform_.scale = easeScale_.easeInCircVec3(bigScale_, smallScale_);

		if (easeScale_.GetIsEnd() == true) {
			tState_ = ScaleSmall;
			easeScale_.Reset();
		}
		break;
	case ScaleSmall:
		easeScale_.SetEaseLimitTime(30);
		easeScale_.Update();

		// スケール変化（大きいから小さい）
		enemy.worldTransform_.scale = easeScale_.easeOutCircVec3(smallScale_, bigScale_);

		if (easeScale_.GetIsEnd() == true) {
			tState_ = ScaleBig;
			easeScale_.Reset();
		}
		break;
	default:
		break;
	}

	if (enemy.worldTransform_.scale.x >= 1.0f) {
		enemy.worldTransform_.scale.x -= 0.01f;
		enemy.worldTransform_.scale.y -= 0.01f;
	}

	enemy.worldTransform_.translation += followVec;
	enemy.worldTransform_.translation.y = 1.0f;
	enemy.worldTransform_.rotation.y += MathUtil::DegreeToRadian(5);
}
