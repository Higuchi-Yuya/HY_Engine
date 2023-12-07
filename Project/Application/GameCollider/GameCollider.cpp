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

	boxModel_.reset(Model::LoadFromOBJ("box1x1x1"));

	for (size_t i = 0; i < 4; i++)
	{
		box_[i].reset(Object3d::Create());
		box_[i]->SetModel(boxModel_.get());
		box_[i]->worldTransform_.translation.y = 1;
		switch (i)
		{
		case 0:
			box_[i]->worldTransform_.translation.z = 30 + 50.22f;
			box_[i]->worldTransform_.scale.x = 50;
			box_[i]->worldTransform_.scale.z = 50;
			break;
		case 1:
			box_[i]->worldTransform_.translation.z = -30 - 50.22f;
			box_[i]->worldTransform_.scale.x = 50;
			box_[i]->worldTransform_.scale.z = 50;
			break;
		case 2:
			box_[i]->worldTransform_.translation.x = 30 + 45.8f;
			box_[i]->worldTransform_.scale.x = 30;
			box_[i]->worldTransform_.scale.z = 30;
			break;
		case 3:
			box_[i]->worldTransform_.translation.x = -30 - 47.3f;
			box_[i]->worldTransform_.scale.x = 30;
			box_[i]->worldTransform_.scale.z = 30;
			break;
		default:
			break;
		}
		box_[i]->worldTransform_.UpdateMatrix();
	}



}

void GameCollider::Updata()
{
	//寿命が尽きた敵の情報を全削除
	enemysInfo_.erase(std::remove_if(enemysInfo_.begin(), enemysInfo_.end(), [](Enemy* x) {
		return x->GetAlive() == false;
		}), enemysInfo_.end());

	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();

	// プレイヤーの情報をスフィアのものに登録
	Sphere p;
	p.center = player_->worldTransform_.translation;
	p.center.y += 1.0f;
	p.radius = 1.0f;

	// 当たり判定（エネミー側の反応）
	for (auto& e : enemysInfo_) {
		// エネミーの情報をスフィアのものに登録
		Sphere sphereE;
		sphereE.center = e->worldTransform_.translation;
		sphereE.radius = 1.0f;

		if (e->GetState() == Enemy::State::Alive && Collision::CheckOBB(player_->worldTransform_, e->worldTransform_)) {
			// エネミーがわを赤くする（仮）
			e->worldTransform_.color = { 1,0,0,1 };
			//e->SetAlive(false);
			// プレイヤーのヒットフラグを立てる
			isPlayerHit = true;

		}
		else {
			player_->OnColUpSpeed();
			e->worldTransform_.color = { 1,1,1,1 };
		}

		// -------------敵とプレイヤーの弾の当たり判定------------ //
		for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {

			// プレイヤーの弾の情報をスフィアのものに登録
			Sphere pB;
			pB.center = playerbullet->worldTransform_.translation;
			pB.radius = 1.0f;

			// プレイヤーの弾とエネミーの当たり判定
			if (e->GetState() == Enemy::State::Alive && Collision::CheckSphere2Sphere(sphereE, pB)) {
				// エネミーが死亡
				e->OnCollision();
				// 弾を消す
				playerbullet->OnCollision();
			}
		}

		// ----------敵と敵同士の当たり判定-------------- //
		for (auto& e2 : enemysInfo_) {
			if (e2 != e) {
				Sphere sphereE2;
				sphereE2.center = e2->worldTransform_.translation;
				sphereE2.radius = 1.0f;

				if (e->GetState() == Enemy::State::Alive && Collision::CheckSphere2Sphere(sphereE, sphereE2, &e->interPos, &e->rejectVec)) {
					e->pushBackOnCol();
				}
			}
		}


		// -----------プレイヤーと敵の当たり判定------------- //
		if (e->GetState() == Enemy::State::Alive && Collision::CheckSphere2Sphere(sphereE, p, &e->interPos, &e->rejectVec)) {
			e->pushBackOnCol();

			// プレイヤーの前方情報をスフィアのものに登録
			Sphere frontP;
			frontP.center = player_->GetFrontPos();
			//frontP.center.y += 1.0f;
			frontP.radius = 0.8f;
			if (Collision::CheckSphere2Sphere(sphereE, frontP)) {
				player_->OnColDownSpeed();
			}
			else {
				player_->OnColUpSpeed();
			}
		}

		// -----------敵とフィールドのオブジェクトの当たり判定------------- //
		for (auto obj : objectsInfo_) {
			Sphere sphereE2;
			sphereE2.center = obj->worldTransform_.translation;
			sphereE2.center.y += 1;
			sphereE2.radius = 1.0f;

			if (e->GetState() == Enemy::State::Alive && Collision::CheckSphere2Sphere(sphereE, sphereE2, &e->interPos, &e->rejectVec)) {
				e->pushBackOnCol();
			}
		}
	}

	// フィールドのオブジェクトの当たり判定
	for (auto obj : objectsInfo_) {
		Sphere sphereObj;
		sphereObj.center = obj->worldTransform_.translation;
		sphereObj.center.y = 1.0f;
		sphereObj.radius = 0.6f;

		// -----------プレイヤーとフィールドのオブジェクトの当たり判定----------- //
		if (Collision::CheckSphere2Sphere(p, sphereObj, &player_->interPos, &player_->rejectVec)) {
			player_->pushBackOnCol();
		}

		// -------------プレイヤーの弾とフィールドのオブジェクトの当たり判定------------ //
		for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {

			// プレイヤーの弾の情報をスフィアのものに登録
			Sphere pB;
			pB.center = playerbullet->worldTransform_.translation;
			pB.radius = playerbullet->worldTransform_.scale.x;

			// プレイヤーの弾とエネミーの当たり判定
			if (Collision::CheckSphere2Sphere(sphereObj, pB)) {
				// 弾を消す
				playerbullet->OnCollision();
			}
		}
	}

	// 壁との当たり判定
	for (size_t i = 0; i < 4; i++)
	{
		// ------------プレイヤーと壁の当たり判定---------- //
		if (Collision::CheckSphere2AABB(p, box_[i]->worldTransform_, &player_->interPos, &player_->rejectVec)) {
			player_->pushBackOnCol();
		}

		// ------------敵と壁の当たり判定----------- //
		for (auto& e : enemysInfo_) {
			// エネミーの情報をスフィアのものに登録
			Sphere sphereE;
			sphereE.center = e->worldTransform_.translation;
			sphereE.radius = 1.0f;

			if (Collision::CheckSphere2AABB(sphereE, box_[i]->worldTransform_, &e->interPos, &e->rejectVec)) {
				box_[i]->worldTransform_.color = { 1,0,0,1 };
				e->pushBackOnCol();
			}
		}

		// --------プレイヤーの弾と壁の当たり判定---------- //
		for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {

			// プレイヤーの弾の情報をスフィアのものに登録
			Sphere pB;
			pB.center = playerbullet->worldTransform_.translation;
			pB.radius = 1.0f;

			if (Collision::CheckSphere2AABB(pB, box_[i]->worldTransform_)) {
				// 弾を消す
				playerbullet->OnCollision();
			}
		}

		box_[i]->Update();
	}

	if (isPlayerHit == true) {
		// ヒット時に一度パーティクルを出す
		if (isPartile == false) {
			OnColParticle();
			player_->OnColHitPoint();
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
		const float md_acc = 0.003f;
		acc.y = -(float)rand() / RAND_MAX * md_acc;

		Vector3 angle = { 0,0,0 };
		// 追加
		particleMan_->Add(ParticleManager::Type::Normal, 120, pos, vel, acc, angle, 0.2f, 0.0f, startColor_, endColor_);
	}
}

void GameCollider::Draw(ViewProjection* viewProjection)
{
	// ----------------パーティクルの描画はここから--------------- //
	particleMan_->Draw(*viewProjection);

	// ----------------パーティクルの描画ここまで----------------- //
}

void GameCollider::Draw3D(ViewProjection* viewProjection)
{
	[[maybe_unused]] ViewProjection* v = viewProjection;
	
	for (size_t i = 0; i < 4; i++)
	{
		//box_[i]->Draw(viewProjection);
	}
}

void GameCollider::AddEnemy(Enemy* enemy)
{
	enemysInfo_.push_back(enemy);
}

void GameCollider::AddObj(Object3d* obj)
{
	objectsInfo_.push_back(obj);
}

void GameCollider::SetPlayer(Player* player)
{
	player_ = player;
}

void GameCollider::Reset()
{
	enemysInfo_.clear();
}
