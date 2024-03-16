#include "GameLight.h"
#include "Player.h"

void GameLight::Initialize()
{
	// メンバ変数の値を初期化
	spotDownPosY = 0.4f;

	circleShadowCasterPos = { 0,0,0 };

	flashCount_ = 0;
	flashCountMax_ = 3;

	flashDefuValue_ = 1;
	flashMiddleValue_ = 20;
	flashMaxValue_ = 200;
	flashNowValue_ = 1;

	easeFlashMiddleTimeLimit_ = 10;
	easeFlashMaxTimeLimit_ = 30;

	playerPointLightIndensity_ = 0.8f;
	playerPointLightRadius_ = 2;
	playerPointLightDecay_ = 1;
}

void GameLight::Update()
{
	lightGroup_->SetSpotLightActive(0, true);
	lightGroup_->SetCircleShadowActive(0, true);
	lightGroup_->SetPointLightActive((int)(pointLightsInfo_.size() + 1), true);

	Vector3 spotPos = player_->GetWorldPosition();
	spotPos.y += 1;

	Vector3 playerFrontVec = player_->GetFrontVec();
	playerFrontVec.y -= spotDownPosY;
	playerFrontVec.normalize();

	lightGroup_->SetSpotLightDir(0, playerFrontVec);
	lightGroup_->SetSpotLightPos(0, spotPos);

	lightGroup_->SetPointLightPos((int)(pointLightsInfo_.size() + 1), spotPos);

	lightGroup_->SetPointLightIndensity((int)(pointLightsInfo_.size() + 1), playerPointLightIndensity_);
	lightGroup_->SetPointLightRadius((int)(pointLightsInfo_.size() + 1), playerPointLightRadius_);
	lightGroup_->SetPointLightDecay((int)(pointLightsInfo_.size() + 1), playerPointLightDecay_);

	// プレイヤーのフラッシュの処理
	float easeValue;

	switch (flashState_)
	{
	case GameLight::None:
		lightGroup_->SetSpotLightIndensity(0, flashDefuValue_);

		if (player_->GetIsFlash() == true) {
			flashState_ = Shine;
		}
		break;
	case GameLight::Shine:
		// フラッシュのカウントが最大なら
		if (flashCount_ >= flashCountMax_ - 1) {
			easeValue = easeFlash_.easeInCirc(flashDefuValue_, flashMaxValue_);
			easeFlash_.SetEaseLimitTime(easeFlashMaxTimeLimit_);
		}
		else {// それ以外のとき
			easeValue = easeFlash_.easeInCirc(flashDefuValue_, flashMiddleValue_);
			easeFlash_.SetEaseLimitTime(easeFlashMiddleTimeLimit_);
		}
		easeFlash_.Update();

		lightGroup_->SetSpotLightIndensity(0, easeValue);

		// 途中で処理が切り替わる場合
		if (player_->GetIsFlash() == false) {
			flashNowValue_ = easeValue;
			easeFlash_.Reset();
			flashState_ = BackDefu;
			break;
		}

		//イージングが終了したら
		if (easeFlash_.GetIsEnd() == true) {
			easeFlash_.Reset();
			flashState_ = Atten;
			if (flashCount_ >= flashCountMax_ - 1) {
				player_->SetIsFlashMax(true);
			}
		}

		break;
	case GameLight::Atten:
		// フラッシュのカウントが最大なら
		if (flashCount_ >= flashCountMax_ - 1) {
			easeValue = easeFlash_.Out(flashMaxValue_, flashDefuValue_);
			easeFlash_.SetEaseLimitTime(easeFlashMaxTimeLimit_);
		}
		else {// それ以外のとき
			easeValue = easeFlash_.Out(flashMiddleValue_, flashDefuValue_);
			easeFlash_.SetEaseLimitTime(easeFlashMiddleTimeLimit_);
		}
		easeFlash_.Update();

		lightGroup_->SetSpotLightIndensity(0, easeValue);

		// 途中で処理が切り替わる場合
		if (player_->GetIsFlash() == false) {
			flashNowValue_ = easeValue;
			easeFlash_.Reset();
			flashState_ = BackDefu;
			break;
		}

		//イージングが終了したら
		if (easeFlash_.GetIsEnd() == true) {
			easeFlash_.Reset();
			flashCount_++;
			if (flashCount_ >= flashCountMax_) {
				flashCount_ = 0;
			}
			flashState_ = Shine;
		}
		break;
	case GameLight::BackDefu:
		easeFlash_.SetEaseLimitTime(15);
		easeFlash_.Update();

		easeValue = easeFlash_.easeInCirc(flashNowValue_, flashDefuValue_);

		lightGroup_->SetSpotLightIndensity(0, easeValue);

		//イージングが終了したら
		if (easeFlash_.GetIsEnd() == true) {
			flashCount_ = 0;
			easeFlash_.Reset();
			player_->SetIsFlashMax(false);
			flashState_ = FlashState::None;
		}

		break;
	}


	// ライトの更新処理
	circleShadowCasterPos = player_->GetWorldPosition();
	lightGroup_->SetCircleShadowCasterPos(0, circleShadowCasterPos);
	lightGroup_->Update();
}

void GameLight::SetLightGroup(LightGroup* lightGroup)
{
	lightGroup_ = lightGroup;
}

void GameLight::SetPointInfo(std::vector<WorldTransform*> pointLightsInfo)
{
	pointLightsInfo_ = pointLightsInfo;
}

void GameLight::SetPlayer(Player* player)
{
	player_ = player;
}
