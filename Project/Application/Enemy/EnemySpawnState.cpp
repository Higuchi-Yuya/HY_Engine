#include "EnemySpawnState.h"
#include "EnemyAliveState.h"
#include "Enemy.h"

void EnemySpawnState::Update(Enemy& enemy)
{
	// スポーンの最初のポジションがセットされたら
	if (spawnFirstPos_ != 0) {
		ease_.SetEaseLimitTime(240);

		float spawnPy = (float)ease_.In(spawnFirstPos_.y, spawnEndPos_.y);
		ease_.Update();
		enemy.worldTransform_.translation.y = spawnPy;
		//worldTransform_.translation = spawnEndPos_;
		enemy.ParticleUpdate();

		// スポーンが終わったら
		if (ease_.GetIsEnd() == true) {
			ease_.Reset();
			enemy.ChageState(Enemy::StateType::Alive);
		}
	}
	else {
		// 最初のスポーンポジションのセット
		spawnFirstPos_ = enemy.worldTransform_.translation;
		spawnEndPos_ = spawnFirstPos_;
		spawnEndPos_.y += 2.5f;
	}
}
