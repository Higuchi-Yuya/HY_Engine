#include "GameLight.h"
#include "Player.h"
#include "ItemPaper.h"
#include "Random.h"

GameLight::~GameLight()
{
	for (auto g : guidLights_)
	{
		delete g;
	}
	guidLights_.clear();
}

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

	guidLightNum_ = 25;
	guidObjScale_ = { 0.1f,0.1f ,0.1f };
	guidLightIdensity_ = { 0.6f,2.0f,0.2f };
	guidLightRadius_ = { 1.0f,4.0f,2.0f };
	guidLightColor_[0] = { 0.3f,0.55f,0.8f };
	guidLightColor_[1] = { 0.4f,0.8f,0.8f };
	guidLightColor_[2] = { 0.65f,0.8f,0.8f };

	guidModel_.reset(Model::LoadFromOBJ("sphere",true));

	for (size_t i = 0; i < guidLightNum_; i++)
	{
		GuidingLight* g = new GuidingLight();
		g->startTaiming = Random::RangeF(3, 30);
		g->guidlightObj.reset(Object3d::Create());
		g->guidlightObj->SetModel(guidModel_.get());
		g->guidlightObj->worldTransform_.scale = guidObjScale_;
		guidLights_.push_back(g);
	}
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

void GameLight::DrawForward3D(ViewProjection* view)
{
	for (size_t i = 0; i < guidLightNum_; i++)
	{
		guidLights_[i]->guidlightObj->Draw(view);
	}
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

void GameLight::GuidLightUpdate(std::vector<ItemPaper*> itemPapers)
{
	guidLightStartNum_ = (int)(pointLightsInfo_.size() + 2);

	for (size_t i = 0; i < guidLightNum_; i++)
	{
		int pointLightNum = (int)(guidLightStartNum_ + i);

		switch (guidLights_[i]->state)
		{
		case ShiftTime:// 最初の生成のタイミングをずらすフェーズ
			guidLights_[i]->startTimer++;
			guidLights_[i]->guidlightObj->worldTransform_.translation = player_->worldTransform_.translation;

			if (guidLights_[i]->startTimer >= guidLights_[i]->startTaiming) {
				guidLights_[i]->state = SetValue;
			}
			break;
		case SetValue:// 各種移動等に必要な値を設定
		{
			guidLights_[i]->guidlightObj->worldTransform_.translation = player_->worldTransform_.translation;
			Vector3 vel;
			float length = 0;
			float minLength = 9999;

			// アイテムのなかで一番近いものの座標とベクトルの長さを抽出
			for (auto j : itemPapers)
			{
				if (j->GetIsCheckItem() == false) {
					vel = j->billTex_.worldTransform_.translation - player_->worldTransform_.translation;
					length = vel.length();

					if (length < minLength) {
						guidLights_[i]->startPos = player_->worldTransform_.translation;
						guidLights_[i]->endPos = j->billTex_.worldTransform_.translation;
						guidLights_[i]->halfVel = vel / 2;
						guidLights_[i]->easeGuid.SetEaseLimitTime(60);
						minLength = length;
					}
				}
			}
			guidLights_[i]->state = Move;
			break;
		}
		case Move:// 移動処理
		{
			guidLights_[i]->easeGuid.Update();

			if (guidLights_[i]->IsShift == false)
			{
				guidLights_[i]->IsShift = true;
				guidLights_[i]->shiftVel.x = Random::RangeF(-3.0f, 3.0f);
				guidLights_[i]->shiftVel.y = Random::RangeF(1.0f, 5.0f);
				guidLights_[i]->shiftVel.z = Random::RangeF(-3.0f, 3.0f);

				guidLights_[i]->halfVel += guidLights_[i]->shiftVel;
				guidLights_[i]->colPoint = guidLights_[i]->startPos + guidLights_[i]->halfVel;
			}

			// ベジエで移動
			Vector3 BeziersVel = guidLights_[i]->easeGuid.LerpBezireQuadratic(guidLights_[i]->startPos, guidLights_[i]->colPoint, guidLights_[i]->endPos);
			float lightIndensity = guidLights_[i]->easeGuid.LerpBezireQuadratic(guidLightIdensity_.x, guidLightIdensity_.y, guidLightIdensity_.z);
			float lightRadius = guidLights_[i]->easeGuid.LerpBezireQuadratic(guidLightRadius_.x, guidLightRadius_.y, guidLightRadius_.z);
			Vector3 lightColor= guidLights_[i]->easeGuid.LerpBezireQuadratic(guidLightColor_[0], guidLightColor_[1], guidLightColor_[2]);
			guidLights_[i]->guidlightObj->worldTransform_.translation = BeziersVel;
			guidLights_[i]->guidlightObj->worldTransform_.color = Vector4(lightColor.x, lightColor.y, lightColor.z, 0.3f);

			// ポイントライトの情報を更新
			lightGroup_->SetPointLightActive(pointLightNum, true);
			lightGroup_->SetPointLightColor(pointLightNum, lightColor);
			lightGroup_->SetPointLightPos(pointLightNum, BeziersVel);
			lightGroup_->SetPointLightIndensity(pointLightNum, lightIndensity);
			lightGroup_->SetPointLightRadius(pointLightNum, lightRadius);
			lightGroup_->SetPointLightDecay(pointLightNum, playerPointLightDecay_);

			// ベジエが終了したらリセットをして再度始める準備をする
			if (guidLights_[i]->easeGuid.GetIsEnd() == true) {
				guidLights_[i]->easeGuid.Reset();
				guidLights_[i]->state = ShiftTime;
				guidLights_[i]->IsShift = false;
			}

			break;
		}

		}

		// オブジェクトの情報更新
		guidLights_[i]->guidlightObj->Update();
	}

	// ライトグループの更新
	lightGroup_->Update();
}
