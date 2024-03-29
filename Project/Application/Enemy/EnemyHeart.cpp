#include "EnemyHeart.h"
#include "MathUtil.h"

void EnemyHeart::Initialize()
{
	// モデルの初期化
	heartModel_.reset(Model::LoadFromOBJ("heart"));

	// オブジェクトの初期化
	heartObj_.reset(Object3d::Create());
	heartShadowObj_.reset(Object3d::Create());

	heartObj_->SetModel(heartModel_.get());
	heartShadowObj_->SetModel(heartModel_.get());
	
	heartObj_->worldTransform_.color.w = 1.0f;
	heartObj_->worldTransform_.scale = { 0,0,0 };
	heartObj_->worldTransform_.UpdateMatrix();

	// アルファ値を設定
	heartShadowObj_->worldTransform_.scale = { 0,0,0 };
	heartShadowObj_->worldTransform_.color.w = 0.2f;
	heartShadowObj_->worldTransform_.UpdateMatrix();

	// 状態の設定
	state_ = Spawn;
	scaleState_ = Expansion;
	// 蠢きのステータスの設定
	beatDefuScale_ = { 1.5f,1.5f ,1.5f };
	beatScale_ = { 0.2f,0.2f, 0.2f };

	writhingTimer_ = 0;
	writhingTimeMax_ = 60;

	easeHeart_.SetEaseLimitTime(30);
	defuScale_ = { 1.5f,1.5f, 1.5f };
	bigScale_ = { 2.5f,2.5f ,2.5f };

	smallValue_ = { 0.02f,0.02f, 0.02f };
	minScale_ = { 0,0,0 };
	IsSmallEnd_ = false;
	IsLightEnd_ = false;

	spawnValue_ = { 0.02f,0.02f, 0.02f };
	spawnScale_ = { 1,1,1 };
}

void EnemyHeart::Update(WorldTransform& worldData, bool isLightMoveEnd)
{
	heartObj_->worldTransform_.translation = worldData.translation;
	heartShadowObj_->worldTransform_.translation = worldData.translation;
	IsLightEnd_ = isLightMoveEnd;

	switch (state_)
	{
	case Spawn:
		SpawnUpdate();
		break;
	case Beat:
		BeatUpdate();
		break;
	case BeatReduction:
		BeatReductionUpdate();
		break;
	}

	// 情報更新
	heartObj_->Update();
	heartShadowObj_->Update();
}

void EnemyHeart::Draw(ViewProjection* view)
{
	heartObj_->Draw(view);

	if (IsLightEnd_ == false) {
		heartShadowObj_->Draw(view);
	}
}

void EnemyHeart::SpawnUpdate()
{
	// 生成時にスケールを変化
	heartObj_->worldTransform_.scale += spawnValue_;

	if (heartObj_->worldTransform_.scale.x >= spawnScale_.x) {
		state_ = Beat;
	}
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

	// ライトの移動処理が終了したら縮小フェーズに移行
	if (IsLightEnd_ == true) {
		state_ = BeatReduction;
	}
}

void EnemyHeart::BeatReductionUpdate()
{
	if (IsSmallEnd_ == false) {
		heartObj_->worldTransform_.scale -= smallValue_;
	}

	if (heartObj_->worldTransform_.scale.x <= minScale_.x) {
		IsSmallEnd_ = true;
	}
}
