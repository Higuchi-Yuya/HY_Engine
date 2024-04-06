#include "GameCollider.h"

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
			box_[i]->worldTransform_.translation.z = 30 + 30.0f;
			box_[i]->worldTransform_.scale.x = 75;
			box_[i]->worldTransform_.scale.z = 1;
			break;
		case 1:
			box_[i]->worldTransform_.translation.z = -30 - 14.22f;
			box_[i]->worldTransform_.scale.x = 75;
			box_[i]->worldTransform_.scale.z = 1;
			break;
		case 2:
			box_[i]->worldTransform_.translation.x = 30 + 37.6f;
			box_[i]->worldTransform_.scale.x = 1;
			box_[i]->worldTransform_.scale.z = 65;
			break;
		case 3:
			box_[i]->worldTransform_.translation.x = -30 - 39.0f;
			box_[i]->worldTransform_.scale.x = 1;
			box_[i]->worldTransform_.scale.z = 65;
			break;
		default:
			break;
		}
		box_[i]->worldTransform_.UpdateMatrix();
	}

	testBox_.reset(Object3d::Create());
	testBox_->SetModel(boxModel_.get());
	testBox_->worldTransform_.translation.x = -0.8f;
	testBox_->worldTransform_.translation.y = 1;
	testBox_->worldTransform_.translation.z = -42.5f;
}

void GameCollider::Update()
{
	//寿命が尽きた敵の情報を全削除
	enemysInfo_.erase(std::remove_if(enemysInfo_.begin(), enemysInfo_.end(), [](Enemy* x) {
		return x->GetAlive() == false;
		}), enemysInfo_.end());

	// プレイヤーの情報をスフィアのものに登録
	pcol_.center = player_->worldTransform_.translation;
	pcol_.center.y += 1.0f;
	pcol_.radius = 1.0f;

	Box testB;
	testB.center = testBox_->worldTransform_.translation;
	testB.radius = { 3.6f,1.1f,1.1f };
	testBox_->worldTransform_.scale = testB.radius;
	if (Collision::CheckSphereToAABB2D(pcol_, testB)) {
		testBox_->worldTransform_.color = { 1,0,0,1 };
		IsClear_ = true;
	}
	else {
		testBox_->worldTransform_.color = { 1,1,1,1 };
		IsClear_ = false;
	}
	testBox_->worldTransform_.UpdateMatrix();

	// 敵周りの当たり判定処理
	EnemyCollisionUpdate();

	// フィールドオブジェクトや壁の当たり判定処理
	ObjAronudCollisionUpdate();

	// プレイヤー周りの当たり判定処理
	PlayerCollisionUpdate();

	// アイテム周りの当たり判定
	ItemCollisionUpdate();
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

	// ----------------パーティクルの描画ここまで----------------- //3つのかけらを集め、脱出せよ
}

void GameCollider::Draw3D(ViewProjection* viewProjection)
{
	[[maybe_unused]] ViewProjection* v = viewProjection;
	
	for (size_t i = 0; i < 4; i++)
	{
		//box_[i]->Draw(viewProjection);
	}

	//testBox_->Draw(viewProjection);
}

void GameCollider::AddEnemy(Enemy* enemy)
{
	enemysInfo_.push_back(enemy);
}

void GameCollider::AddObj(DeferredObject3d* obj)
{
	objectsInfo_.push_back(obj);
}

void GameCollider::AddObjSphere(Sphere sphere)
{
	objSpheres_.push_back(sphere);
}

void GameCollider::AddObjBox(Box box)
{
	objBox_.push_back(box);
}

void GameCollider::AddItem(ItemPaper* item)
{
	ItemPapersInfo_.push_back(item);
}

void GameCollider::SetPlayer(Player* player)
{
	player_ = player;
}

void GameCollider::Reset()
{
	enemysInfo_.clear();
	IsClear_ = false;
}

void GameCollider::EnemyCollisionUpdate()
{
	// プレイヤーのバレット情報を格納する配列
	std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();

	// 当たり判定（エネミー側の反応）
	for (auto& e : enemysInfo_) {
		// エネミーの情報をスフィアのものに登録
		Sphere sphereE;
		sphereE.center = e->worldTransform_.translation;
		sphereE.radius = 1.0f;

		Sphere sphereERange;
		sphereERange.center = e->worldTransform_.translation;
		sphereERange.radius = 7.0f;
		if (e->GetState() == Enemy::StateType::Alive && Collision::CheckSphere2Sphere(pcol_, sphereERange)) {
			e->SetAliveState(State::AliveState::Tracking);
			e->NearPlayerParticleUpdate();
			e->ResetBack();
			if (Collision::CheckOBB(player_->worldTransform_, e->worldTransform_)) {
				// エネミーがわを赤くする（仮）
				e->worldTransform_.color = { 1,0,0,1 };
				//e->SetAlive(false);
				// プレイヤーのヒットフラグを立てる
				isPlayerHit = true;

			}
		}
		else if(e->GetAliveState()!= State::AliveState::Patrol) {
			player_->OnColUpSpeed();
			e->SetAliveState(State::AliveState::Back);
			e->worldTransform_.color.y = 1;
			e->worldTransform_.color.z = 1;
		}

		// -------------敵とプレイヤーの弾の当たり判定------------ //
		for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {

			// プレイヤーの弾の情報をスフィアのものに登録
			Sphere pB;
			pB.center = playerbullet->worldTransform_.translation;
			pB.radius = 1.0f;

			// プレイヤーの弾とエネミーの当たり判定
			if (e->GetState() == Enemy::StateType::Alive && Collision::CheckSphere2Sphere(sphereE, pB)) {
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

				if (e->GetState() == Enemy::StateType::Alive && Collision::CheckSphere2Sphere(sphereE, sphereE2, &e->interPos, &e->rejectVec)) {
					e->pushBackOnCol();
				}
			}
		}


		// -----------プレイヤーと敵の当たり判定------------- //
		if (e->GetState() == Enemy::StateType::Alive && Collision::CheckSphere2Sphere(sphereE, pcol_, &e->interPos, &e->rejectVec)) {
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

		// -----------プレイヤーの懐中電灯の範囲との当たり判定------------- //
		Cone flashCone;
		flashCone.vertexPos = player_->worldTransform_.translation;
		flashCone.directionVec = player_->GetFrontVec();
		flashCone.height = 14;
		flashCone.radius = 10;

		if (e->GetState() == Enemy::StateType::Alive &&
			Collision::CheckSphereToCone(sphereE, flashCone) &&
			player_->GetIsFlashMax() == true) {

			e->SetIsAlive(false);
		}

		// -----------敵とフィールドのオブジェクトの当たり判定------------- //
		for (auto obj : objectsInfo_) {
			Sphere sphereE2;
			sphereE2.center = obj->worldTransform_.translation;
			sphereE2.center.y += 1;
			sphereE2.radius = 1.0f;

			if (e->GetState() == Enemy::StateType::Alive && Collision::CheckSphere2Sphere(sphereE, sphereE2, &e->interPos, &e->rejectVec)) {
				e->pushBackOnCol();
			}
		}
	}
}

void GameCollider::PlayerCollisionUpdate()
{
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

void GameCollider::ObjAronudCollisionUpdate()
{
	// プレイヤーのバレット情報を格納する配列
	std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();

	// フィールドのオブジェクトの当たり判定
	for (auto obj : objectsInfo_) {
		Sphere sphereObj;
		sphereObj.center = obj->worldTransform_.translation;
		sphereObj.center.y = 1.0f;
		sphereObj.radius = 0.6f;

		// -----------プレイヤーとフィールドのオブジェクトの当たり判定----------- //
		if (Collision::CheckSphere2Sphere(pcol_, sphereObj, &player_->interPos, &player_->rejectVec)) {
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
		Box wallbox;
		wallbox.center = box_[i]->worldTransform_.translation;
		wallbox.radius = box_[i]->worldTransform_.scale;
		// ------------プレイヤーと壁の当たり判定---------- //
		if (Collision::CheckSphereToAABB2D(pcol_, wallbox, &player_->worldTransform_)) {
			//player_->pushBackOnCol();
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

	for (auto objSphere : objSpheres_)
	{
		// ------------プレイヤーと追加オブジェクトの当たり判定---------- //
		if (Collision::CheckSphere2Sphere(pcol_, objSphere, &player_->interPos,&player_->rejectVec)) {
			player_->pushBackOnCol();
		}
	}

	for (auto objBox : objBox_)
	{
		// ------------プレイヤーと追加オブジェクトの当たり判定---------- //
		if (Collision::CheckSphereToAABB2D(pcol_, objBox, &player_->worldTransform_)) {
			//player_->pushBackOnCol();
		}
	}
}

void GameCollider::ItemCollisionUpdate()
{
	for (auto item : ItemPapersInfo_)
	{
		// アイテムビルボードオブジェの情報をスフィアに設定
		Sphere sItemRange;
		sItemRange.center = item->billTex_.worldTransform_.translation;
		sItemRange.radius = 8.0f;

		Sphere sItemInRange;
		sItemInRange.center = item->billTex_.worldTransform_.translation;
		sItemInRange.radius = 1.0f;

		// アイテムのキラキラが表示される範囲だったら
		if (Collision::CheckSphere2Sphere(pcol_, sItemRange) && item->GetIsCheckItem() == false) {

			player_->SetIsItemRange(true);

			// アイテムが取得できる範囲だったら
			if (Collision::CheckSphere2Sphere(pcol_, sItemInRange)) {
				player_->SetIsItemInRange(true);
				break;
			}

			break;
		}
		else {
			player_->SetIsItemRange(false);
			player_->SetIsItemInRange(false);
		}
	}
}
