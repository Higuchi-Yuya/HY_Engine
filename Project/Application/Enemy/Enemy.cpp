#include "Enemy.h"
#include "Collision.h"
#include "MathUtil.h"
#include "Easing.h"
#include "Random.h"
#include "EnemySpawnState.h"
#include "EnemyAliveState.h"

std::unique_ptr<Texture> Enemy::sParticleTex_ = nullptr;
std::unique_ptr<Model> Enemy::modelDefu_ = nullptr;

Enemy::~Enemy()
{
	delete curenntState_;
}

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
	worldTransform_.translation.y -= 2.5f;
	worldTransform_.UpdateMatrix();

	// パーティクルのマネージャの初期化
	particleMan_ = std::make_unique<ParticleManager>();
	particleMan_->Initialize();
	particleMan_->SetTextureHandle(sParticleTex_.get());

	// ライフの初期化
	nowLife_ = maxLife_;

	curenntState_ = new EnemySpawnState();
}

void Enemy::Update()
{
	// 今の状態の更新処理
	curenntState_->Update(*this);

	// パーティクルの更新処理
	particleMan_->Update();

	// 行列の更新など
	Object3d::Update();
}

void Enemy::ParticleUpdate()
{
	if (curenntState_->GetSpawnTimer() < curenntState_->GetSpawnTimeMax() - 30) {
		// スポーンするときのパーティクル
		for (int i = 0; i < particleNum; i++) {

			Vector3 pos{};
			// ポジションをエネミーの中心座標にセット
			pos = curenntState_->GetSpawnFirstPos();
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
	}

	// 当たり判定
	for (int i = 0; i < particleNum; i++) {
		// X,Y,Z全て{-5.0f,+5.0f}でランダムに分布
		Vector3 pos{};

		// ポジションをプレイヤーの中心座標にセット
		pos = worldTransform_.translation;

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

void Enemy::ResetBack()
{
	curenntState_->SetIsAlphaZero(false);
	worldTransform_.color.w = 1;

	curenntState_->SetIsPatrolEnd(false);
	curenntState_->SetPatrolNum(0);
	curenntState_->EaseReset();
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

void Enemy::SetPlayer(Player* player)
{
	IEnemyState::SetPlayer(player);
}

void Enemy::SetIsAlive(const bool isAlive)
{
	curenntState_->SetIsAlive(isAlive);
}

void Enemy::SetAliveState(IEnemyState::AliveState aState)
{
	curenntState_->SetAliveState(aState);
}

void Enemy::ChageState(StateType stateType)
{
	delete curenntState_;
	switch (stateType)
	{
	case Enemy::StateType::Spawn:
		nowState_ = StateType::Spawn;
		curenntState_ = new EnemySpawnState();
		break;
	case Enemy::StateType::Alive:
		nowState_ = StateType::Alive;
		curenntState_ = new EnemyAliveState();
		break;
	default:
		break;
	}
	curenntState_->PatrolInit(*this);
}

void Enemy::NearPlayerParticleUpdate()
{
	curenntState_->IncrementNearTimer();
	if (curenntState_->GetNearTimer() > curenntState_->GetNearTimeLimit()) {
		curenntState_->SetNearTimer(0);
		// スポーンするときのパーティクル
		for (int i = 0; i < curenntState_->GetNearParticleNum(); i++) {

			Vector3 pos{};
			// ポジションをエネミーの中心座標にセット
			//pos.y += 1.5f;

			// X,Y,Z全てでランダムに分布
			Vector3 vel{};

			const float md_angle = 360.0f;
			Vector3 angle{};
			float randAngle = (float)rand() / RAND_MAX * md_angle - md_angle;
			angle.x = randAngle;
			angle.z = randAngle;
			angle.x += MathUtil::DegreeToRadian(angle.x);
			angle.z += MathUtil::DegreeToRadian(angle.z);
			float range = Random::RangeF(0.1f, 1.0f);

			pos.y = worldTransform_.translation.y + Random::RangeF(-0.6f, 0.6f);
			//range = abs(pos.y);
			// 角度の方向にポジションをセット
			pos.x = worldTransform_.translation.x + cos(angle.x) * range;
			pos.z = worldTransform_.translation.z + sin(angle.z) * range;

			// 重力に見立ててYのみでランダムに分布
			Vector3 acc{};
			const float md_acc = 0.0015f;
			
			acc.x = Random::RangeF(-md_acc, md_acc);
			acc.y = Random::RangeF(-md_acc, md_acc);
			acc.z = Random::RangeF(-md_acc, md_acc);

			// 追加
			particleMan_->Add(ParticleManager::Type::EnemyNear, 150, pos, vel, acc, angle, 0.1f, 0.0f, curenntState_->GetNearStartColor(), curenntState_->GetNearEndColor());
		}
	}
}
