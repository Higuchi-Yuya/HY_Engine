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

	postScaling_ = std::make_unique<PostEffectScaling>();
	postScaling_->Initialize();

	beatDistance_ = 10;
	beatInterval_ = 30;

	IsBeat_ = true;
	IsNotBeatDo_ = false;

	beatState_ = _Expansion;
	notBeatState_ = _SetValue;

	beatExpansionStart_ = { 1.0f,1.0f };
	beatExpansionEnd_ = { 0.96f,0.96f };

	beatReductionStart_ = beatExpansionEnd_;
	beatReductionEnd_ = beatExpansionStart_;

	timer_ = 0;
	timeLimit_ = 60 * 4;

	postRipples_->centerPos_ = { 0.5f,0.7f };
	postRipples_->waveSpan_ = 20;
	postRipples_->waveScale_ = 0.01f;
}

void BeatEffect::Update()
{
	// 近くに敵がいるかどうかの処理
	NearUpdate();

	// 鼓動の更新処理
	BeatUpdate();

	// 鼓動しないときの更新処理
	NotBeatUpdate();

	// 合成用ポストエフェクトの更新処理
	postComposition_->Update();

	// 拡大縮小の更新処理
	postScaling_->Update();

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

		postScaling_->tailing_ = tiling;

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

		postScaling_->tailing_ = tiling;

		if (ease_.GetIsEnd() == true) {
			ease_.Reset();
			beatState_ = _Expansion;
		}
		break;
	}
	default:
		break;
	}
}

void BeatEffect::NotBeatUpdate()
{
	if (IsBeat_ == true) {
		return;
	}

	// タイリングの情報が　1.0f　未満の場合
	// イージングの残り時間を使い戻してあげる
	if (postScaling_->tailing_.x < 1.0f) {
		switch (notBeatState_)
		{
		case BeatEffect::_SetValue:
		{
			// 値の設定をする
			int32_t LTime = ease_.GetLimitTime() - ease_.GetTimer();
			ease_.SetEaseLimitTime(LTime);

			notBeatStart_ = postScaling_->tailing_;
			notBeatEnd_ = { 1.0f,1.0f };

			notBeatState_ = _Undo;
			break;
		}
		case BeatEffect::_Undo:
		{
			ease_.Update();

			Vector2 tiling = { ease_.In(notBeatStart_.x,notBeatEnd_.x),
							   ease_.In(notBeatStart_.y,notBeatEnd_.y) };

			postScaling_->tailing_ = tiling;

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

void BeatEffect::NearUpdate()
{
	// ビートフラグがオンの時
	// 基本的にずっと処理を繰り返す

	// 凄く近い時
	for (auto& e : enemysInfo_) {

		Vector3 vec = e->worldTransform_.translation - player_->worldTransform_.translation;
		float dis = vec.length();

		// 敵とプレイヤーのベクトルをとり、長さが一定以下の場合鼓動のフラグをオンして
		// ループを抜ける
		float nearDistance = beatDistance_ * 0.5f;
		if (dis <= nearDistance) {
			IsBeat_ = true;
			IsNear_ = true;
			beatInterval_ = 15;
			break;
		}
		// それ以外は鼓動のフラグをオフ
		else {
			IsBeat_ = false;
			IsNear_ = false;
		}
	}

	// ある程度の近さの時
	if (IsNear_ == false) {
		for (auto& e : enemysInfo_) {

			Vector3 vec = e->worldTransform_.translation - player_->worldTransform_.translation;
			float dis = vec.length();
			if (dis <= beatDistance_) {
				IsBeat_ = true;
				beatInterval_ = 30;
				break;
			}
			else {
				IsBeat_ = false;
			}
		}
	}
}

void BeatEffect::ImguiUpdate()
{
	// 表示項目の追加--------//
	ImGui::Begin("Ripples");

	ImGui::SetNextWindowSize(ImVec2(500, 100));

	ImGui::InputFloat2("centerPos", &postRipples_->centerPos_.x);

	ImGui::InputFloat("alpha", &postRipples_->alpha_);
	ImGui::InputFloat("waveSpan", &postRipples_->waveSpan_);
	ImGui::InputFloat("waveFrame", &postRipples_->waveFrame_);
	ImGui::InputFloat("waveScale", &postRipples_->waveScale_);
	ImGui::InputFloat("timeLimit", &timeLimit_);

	ImGui::InputFloat2("scalingTailing", &postScaling_->tailing_.x);
	ImGui::InputFloat2("scalingOffset", &postScaling_->offset_.x);
	ImGui::InputFloat("beatInterval", &beatInterval_);

	ImGui::End();

	postComposition_->ImguiUpdate();
	postRipples_->ImguiUpdate();
}

void BeatEffect::DrawPass()
{
	// 合成ポストエフェクト
	postComposition_->PreDrawScene(cmdList_);
	gaussianBlur_->Draw(cmdList_);
	postComposition_->PostDrawScene(cmdList_);

	// 拡大縮小の描画
	postScaling_->PreDrawScene(cmdList_);
	postComposition_->Draw(cmdList_);
	postScaling_->PostDrawScene(cmdList_);
}

void BeatEffect::Draw()
{
	postComposition_->Draw(cmdList_);
	postScaling_->Draw(cmdList_);
}

void BeatEffect::Reset()
{
	// 状態を分別するのをリセット
	IsBeat_ = false;
	IsNotBeatDo_ = false;

	// 状態をリセット
	beatState_ = _Expansion;
	notBeatState_ = _SetValue;

	// 拡縮のタイリングをリセット
	postScaling_->tailing_ = { 1,1 };
	// 拡大縮小の更新処理
	postScaling_->Update();
}
