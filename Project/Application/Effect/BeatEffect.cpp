#include "BeatEffect.h"

GaussianBlur* BeatEffect::gaussianBlur_ = nullptr;
std::vector<Enemy*>BeatEffect::enemysInfo_{};
Player* BeatEffect::player_ = nullptr;
ID3D12GraphicsCommandList* BeatEffect::cmdList_ = nullptr;

void BeatEffect::Initialize()
{
	postComposition_ = std::make_unique<PostEffectComposition>();
	postComposition_->Initialize();

	postRipples_ = std::make_unique<PostEffectRipples>();
	postRipples_->Initialize();

	beatDistance_ = 5;
	beatInterval_ = 60;

	IsBeat_ = true;
	IsNotBeatDo_ = false;

	beatState_ = _Expansion;
	notBeatState_ = _SetValue;

	beatExpansionStart_ = { 1.0f,1.0f };
	beatExpansionEnd_ = { 0.95f,0.95f };

	beatReductionStart_ = { 0.95f,0.95f };
	beatReductionEnd_ = { 1.0f,1.0f };

	timer_ = 0;
	timeLimit_ = 60 * 2;
}

void BeatEffect::Update()
{
	// ビートフラグがオンの時
	// 基本的にずっと処理を繰り返す
	for (auto& e : enemysInfo_) {
		
		Vector3 vec= e->worldTransform_.translation - player_->worldTransform_.translation;
		float dis = vec.length();

		// 敵とプレイヤーのベクトルをとり、長さが一定以下の場合鼓動のフラグをオンして
		// ループを抜ける
		if (dis <= beatDistance_) {
			IsBeat_ = true;
			break;
		}
		// それ以外は鼓動のフラグをオフ
		else {
			IsBeat_ = false;
		}
	}

	// 鼓動の更新処理
	BeatUpdate();

	// 鼓動しないときの更新処理
	NotBeatUpdate();

	// 合成用ポストエフェクトの更新処理
	postComposition_->Update();

	// 波紋の更新処理
	postRipples_->Update();
}

void BeatEffect::BeatUpdate()
{
	if (IsBeat_ == false && IsNotBeatDo_ == true) {
		return;
	}
	if (IsBeat_ == false) {
		return;
	}

	switch (beatState_)
	{
	case BeatEffect::_Expansion:// 膨張状態
	{
		ease_.SetEaseLimitTime((int32_t)(beatInterval_ / 2.0f));
		ease_.Update();

		Vector2 tiling = { ease_.In(beatExpansionStart_.x,beatExpansionEnd_.x),
						   ease_.In(beatExpansionStart_.y,beatExpansionEnd_.y) };

		postComposition_->SetCompoTiling(tiling);

		if (ease_.GetIsEnd() == true) {
			ease_.Reset();
			beatState_ = _Reduction;
		}
		break;
	}
	case BeatEffect::_Reduction:// 縮小状態
	{
		ease_.SetEaseLimitTime((int32_t)(beatInterval_ / 2.0f));
		ease_.Update();

		Vector2 tiling = { ease_.In(beatReductionStart_.x,beatReductionEnd_.x),
						   ease_.In(beatReductionStart_.y,beatReductionEnd_.y) };

		postComposition_->SetCompoTiling(tiling);

		if (ease_.GetIsEnd() == true) {
			ease_.Reset();
			beatState_ = _Expansion;
		}
		break;
	}
	default:
		break;
	}

	// 波紋の処理
	timer_++;
	if (timer_ >= timeLimit_) {
		timer_ = 0;
	}

	float timeRate = timer_ / timeLimit_;

	postRipples_->IsActive_ = true;
	postRipples_->waveFrame_ = timeRate;
}

void BeatEffect::NotBeatUpdate()
{
	if (IsBeat_ == true) {
		return;
	}

	// タイリングの情報が　1.0f　未満の場合
	// イージングの残り時間を使い戻してあげる
	if (postComposition_->GetTiling().x < 0.99f) {
		switch (notBeatState_)
		{
		case BeatEffect::_SetValue:
		{
			// 値の設定をする
			int32_t LTime = ease_.GetLimitTime() - ease_.GetTimer();
			ease_.SetEaseLimitTime(LTime);

			notBeatStart_ = postComposition_->GetTiling();
			notBeatEnd_ = { 1.0f,1.0f };

			notBeatState_ = _Undo;
			break;
		}
		case BeatEffect::_Undo:
		{
			ease_.Update();

			Vector2 tiling = { ease_.In(notBeatStart_.x,notBeatEnd_.x),
							   ease_.In(notBeatStart_.y,notBeatEnd_.y) };

			postComposition_->SetCompoTiling(tiling);

			if (ease_.GetIsEnd() == true) {
				ease_.Reset();
				IsNotBeatDo_ = false;
				notBeatState_ = _SetValue;
			}
			else {
				IsNotBeatDo_ = true;
			}
			break;
		}
		default:
			break;
		}
	}

	// 波紋をオフ
	postRipples_->IsActive_ = false;
}

void BeatEffect::ImguiUpdate()
{
	postComposition_->ImguiUpdate();
	postRipples_->ImguiUpdate();
}

void BeatEffect::DrawPass()
{
	// 合成ポストエフェクト
	postComposition_->PreDrawScene(cmdList_);
	gaussianBlur_->Draw(cmdList_);
	postComposition_->PostDrawScene(cmdList_);

	postRipples_->PreDrawScene(cmdList_);
	// 合成用ポストエフェクトの描画
	postComposition_->Draw(cmdList_);
	postRipples_->PostDrawScene(cmdList_);
}

void BeatEffect::Draw()
{
	postRipples_->Draw(cmdList_);
}
