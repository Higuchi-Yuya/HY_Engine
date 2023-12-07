#include "Enemy.h"
#include "Collision.h"
#include "MathUtil.h"
#include "Easing.h"


std::unique_ptr<Texture> Enemy::sParticleTex_ = nullptr;
Player* Enemy::player_ = nullptr;
std::unique_ptr<Model> Enemy::modelDefu_ = nullptr;

void Enemy::StaticInitialize()
{
	// パーティクルのテクスチャの初期化
	sParticleTex_.reset(TextureManager::Load2DTextureP("effect.png"));
	// 敵のモデル読み込み
	modelDefu_.reset(Model::LoadFromOBJ("Medama", true));
}

void Enemy::StaticFinalize()
{
	sParticleTex_ = nullptr;
	player_ = nullptr;
	modelDefu_ = nullptr;
}

void Enemy::Initialize(Model* model)
{

	Object3d::Initialize();
	// モデルのセット
	if (model) {
		SetModel(model);
	}
	else if (model == nullptr) {
		SetModel(modelDefu_.get());
	}

	IsAlive_ = true;
	worldTransform_.translation.y -= 2.5f;
	//worldTransform_.rotation.y += MathUtil::DegreeToRadian(90);
	//worldTransform_.scale.x = 1.0f;
	worldTransform_.UpdateMatrix();

	// パーティクルのマネージャの初期化
	particleMan_ = std::make_unique<ParticleManager>();
	particleMan_->Initialize();
	particleMan_->SetTextureHandle(sParticleTex_.get());

	// ライフの初期化
	nowLife_ = maxLife_;
}

void Enemy::Update()
{
	switch (nowState_)
	{
	case Enemy::State::Spawn:
		// スポーンの最初のポジションがセットされたら
		if (spawnFirstPos_ != 0) {
			ease_.SetEaseLimitTime(240);
			
			float spawnPy = (float)ease_.In(spawnFirstPos_.y, spawnEndPos_.y);
			ease_.Update();
			worldTransform_.translation.y = spawnPy;
			//worldTransform_.translation = spawnEndPos_;
			ParticleUpdate();

			// スポーンが終わったら
			if (ease_.GetIsEnd() == true) {
				nowState_ = State::Alive;
				ease_.Reset();
			}
		}
		else {
			// 最初のスポーンポジションのセット
			spawnFirstPos_ = worldTransform_.translation;
			spawnEndPos_ = spawnFirstPos_;
			spawnEndPos_.y += 2.5f;
		}
		break;
	case Enemy::State::Alive:
		{
			// 移動前ポジションを代入
			oldPos_ = worldTransform_.translation;

			timer++;
			followTimer++;
			if (timer > maxTime) {
				timer = 0;
			}
			// 借りてきたプレイヤーのポジション
			Vector3 playerPos = player_->worldTransform_.translation;
			if (followTimer > followTimeMax) {
				// 追従のベクトルの更新
				Vector3 velo = playerPos - worldTransform_.translation;
				followVec = velo.normalize() * followSpeed;
				followTimer = 0;
			}

			worldTransform_.translation += followVec;
			worldTransform_.translation.y = 1.0f;
			worldTransform_.rotation.y += MathUtil::DegreeToRadian(5);

			// 死んだら状態を変更
			if (IsAlive_ == false) {
				nowState_ = State::Dead;
			}
		}
		
		break;
	case Enemy::State::Dead:
		disoTimer_++;
		disoTimeLate_ = disoTimer_ / disoTimeMax_;
		dissolve_.isActiveDissolve_ = true;
		dissolve_.dissolveColor_ = Vector4(0.15f, 0.0f, 0.0f, 1);
		dissolve_.dissolveTime_ = disoTimeLate_;

		if (dissolve_.dissolveTime_ >= 1.0f) {
			IsDeadMotionEnd = true;
		}
		break;
	default:
		break;
	}

	particleMan_->Update();
	// 行列の更新など
	Object3d::Update();
}

void Enemy::ParticleUpdate()
{
	if (spawnTimer < spawnTimeMax - 30) {
		// スポーンするときのパーティクル
		for (int i = 0; i < particleNum; i++) {

			Vector3 pos{};
			// ポジションをエネミーの中心座標にセット
			pos = spawnFirstPos_;
			pos.y += 1.5f;

			// X,Y,Z全てでランダムに分布
			Vector3 vel{};

			const float md_angle = 360.0f;
			Vector3 angle{};
			float randAngle = (float)rand() / RAND_MAX * md_angle - md_angle;
			angle.x = randAngle;
			angle.z = randAngle;
			angle.x += MathUtil::DegreeToRadian(angle.x);
			angle.z += MathUtil::DegreeToRadian(angle.z);
			// 角度の方向にポジションをセット
			pos.x = worldTransform_.translation.x + cos(angle.x) * 1.0f;
			pos.z = worldTransform_.translation.z + sin(angle.z) * 1.0f;

			// 重力に見立ててYのみでランダムに分布
			Vector3 acc{};
			const float md_acc = 0.0015f;
			acc.y = (float)rand() / RAND_MAX * md_acc;

			// 追加
			particleMan_->Add(ParticleManager::Type::EnemySpawn, 120, pos, vel, acc, angle, 0.2f, 0.0f, startColor_, endColor_);
		}
	}
}

void Enemy::Draw(ViewProjection* view)
{
	Object3d::Draw(view);
	
}

void Enemy::DrawParticle(ViewProjection* view)
{
	particleMan_->Draw(*view);
}

void Enemy::OnCollision()
{
	// 現在のライフを一つ減らす
	nowLife_--;

	// ライフが0以下になったら死ぬ
	if (nowLife_ <= 0) {
		IsAlive_ = false;
	}

	// 当たり判定
	for (int i = 0; i < particleNum; i++) {
		// X,Y,Z全て{-5.0f,+5.0f}でランダムに分布
		Vector3 pos{};

		// ポジションをプレイヤーの中心座標にセット
		pos = worldTransform_.translation;
		//pos.y += 0.5f;
		// X,Y,Z全て{-0.05f,+0.05f}でランダムに分布
		const float md_vel = 0.15f;
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

void Enemy::pushBackOnCol()
{
	const Vector3 up = { 0,1,0 };
	Vector3 info_ = rejectVec;
	Vector3 rejectDir = info_.normalize();
	float cos = Vector3::dot(rejectDir, up);

	Vector3 move;

	// 地面判定しきい値
	const float threshold = cosf(Vector3::Deg2Rad(30.0f));

	if (-threshold < cos && cos < threshold) {
		//sphere->center += info.reject;
		move += rejectVec;
	}

	worldTransform_.translation += move;
	worldTransform_.UpdateMatrix();
}

void Enemy::SetWorldTransInfo(WorldTransform worldTrans)
{
	worldTransform_.translation = worldTrans.translation;
	worldTransform_.scale = worldTrans.scale;
	worldTransform_.rotation = worldTrans.rotation;
}

void Enemy::SetWorldPos(Vector3 pos)
{
	// ポジションをセット
	worldTransform_.translation = pos;
	// 行列の更新
	worldTransform_.UpdateMatrix();
}
