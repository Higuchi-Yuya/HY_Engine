#include "GameCollider.h"
#include "Collision.h"

void GameCollider::Initialize()
{
	// パーティクルのテクスチャの初期化
	particleTex_.reset(TextureManager::Load2DTextureP("effect.png"));
	// パーティクルのマネージャの初期化
	particleMan_ = std::make_unique<ParticleManager>();
	particleMan_->Initialize();
	particleMan_->SetTextureHandle(particleTex_.get());
}

void GameCollider::Updata()
{
	// 当たり判定（エネミー側の反応）
	for (auto& e : enemys_){
		if (Collision::CheckOBB(player_->worldTransform_, e->worldTransform_)) {
			// エネミーがわを赤くする（仮）
			e->worldTransform_.color = { 1,0,0,1 }; 

			// プレイヤーのヒットフラグを立てる
			isPlayerHit = true;
			
		}
		else {
			e->worldTransform_.color = { 1,1,1,1 };
		}
	}

	
	if (isPlayerHit == true) {
		// ヒット時に一度パーティクルを出す
		if (isPartile == false) {
			OnColParticle();
			isPartile = true;
		}

	}
	// パーティクルが出たあとプレイヤーの無敵時間のタイマーを進める
	if (isPartile == true) {
		playerHitTimer++;

		// 無敵時間を過ぎたらプレイヤーの当たり判定を再びとれるようにする
		if (playerHitTimer > playerHitTimeMax) {
			playerHitTimer = 0;
			isPartile = false;
			isPlayerHit = false;
		}
	}

	// パーティクルの更新処理
	particleMan_->Update();
}

void GameCollider::OnColParticle()
{
	// 当たり判定（プレイヤー側の反応）
	for (int i = 0; i < particleNum; i++) {
		// X,Y,Z全て{-5.0f,+5.0f}でランダムに分布
		//const float md_pos = 0.0f;
		Vector3 pos{};
		//pos.x = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
		//pos.y = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f + 5;
		//pos.z = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;

		// ポジションをプレイヤーの中心座標にセット
		pos = player_->worldTransform_.translation;
		pos.y += 0.5f;
		// X,Y,Z全て{-0.05f,+0.05f}でランダムに分布
		const float md_vel = 0.1f;
		Vector3 vel{};
		vel.x = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;

		// 重力に見立ててYのみ{-0.001f,0}でランダムに分布
		Vector3 acc{};
		const float md_acc = 0.005f;
		acc.y = -(float)rand() / RAND_MAX * md_acc;

		// 追加
		particleMan_->Add(ParticleManager::Type::Normal, 120, pos, vel, acc, 0.2f, 0.0f, startColor_, endColor_);
	}
}

void GameCollider::Draw(ID3D12GraphicsCommandList* commandList, ViewProjection* viewProjection)
{
	// ----------------パーティクルの描画はここから--------------- //
	ParticleManager::PreDraw(commandList);
	particleMan_->Draw(*viewProjection);


	ParticleManager::PostDraw();
	// ----------------パーティクルの描画ここまで----------------- //
}

void GameCollider::AddEnemy(Enemy* enemy)
{
	enemys_.push_back(enemy);
}

void GameCollider::SetPlayer(Player* player)
{
	player_ = player;
}
