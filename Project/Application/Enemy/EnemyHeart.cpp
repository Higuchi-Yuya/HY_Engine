#include "EnemyHeart.h"
#include "MathUtil.h"

void EnemyHeart::Initialize()
{
	// モデルの初期化
	heartModel_.reset(Model::LoadFromOBJ("sphere"));

	// オブジェクトの初期化
	heartObj_.reset(Object3d::Create());
	heartShadowObj_.reset(Object3d::Create());

	heartObj_->SetModel(heartModel_.get());
	heartShadowObj_->SetModel(heartModel_.get());

	// アルファ値を設定
	heartShadowObj_->worldTransform_.color.w = 0.3f;
	heartShadowObj_->worldTransform_.UpdateMatrix();

	// 状態の設定
	state_ = Beat;
	scaleState_ = Expansion;
	// 蠢きのステータスの設定
	beatDefuScale_ = { 1.0f,1.0f ,1.0f };
	beatScale_ = { 0.1f,0.1f, 0.1f };

	writhingTimer_ = 0;
	writhingTimeMax_ = 60;

	easeHeart_.SetEaseLimitTime(30);
	defuScale_ = { 1.0f,1.0f, 1.0f };
	bigScale_ = { 1.3f,1.3f ,1.3f };
}

void EnemyHeart::Update(WorldTransform& worldData)
{
	worldData;
	switch (state_)
	{
	case Beat:
		BeatUpdate();

		break;
	}
}

void EnemyHeart::Draw(ViewProjection* view)
{
	heartObj_->Draw(view);
	heartShadowObj_->Draw(view);
}

void EnemyHeart::BeatUpdate()
{
	writhingTimer_++;
	if (writhingTimer_ >= writhingTimeMax_) {
		writhingTimer_ = 0;
	}

	heartObj_->worldTransform_.scale = MathUtil::Sin_ZeroToOne(beatDefuScale_, writhingTimeMax_, writhingTimer_, beatScale_);

	switch (scaleState_)
	{
	case EnemyHeart::Expansion:
		easeHeart_.Update();

		heartShadowObj_->worldTransform_.scale = easeHeart_.easeInCircVec3(defuScale_, bigScale_);

		if (easeHeart_.GetIsEnd() == true) {
			easeHeart_.Reset();
			scaleState_ = Reduction;
		}
		break;
	case EnemyHeart::Reduction:
		easeHeart_.Update();

		heartShadowObj_->worldTransform_.scale = easeHeart_.easeInCircVec3(bigScale_, defuScale_);

		if (easeHeart_.GetIsEnd() == true) {
			easeHeart_.Reset();
			scaleState_ = Expansion;
		}
		break;
	}

	

	// 情報更新
	heartObj_->Update();
	heartShadowObj_->Update();
}
