#include "GameLight.h"
#include "Player.h"
#include "ItemPaper.h"
#include "Random.h"
#include "Enemy.h"

std::vector<Enemy*>GameLight::enemysInfo_{};

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

	guidLightNum_ = 26;
	lightOffPos_ = { 0,1000,0 };
	guidObjScale_ = { 0.1f,0.1f ,0.1f };
	guidLightIdensity_ = { 1.0f,1.0f,1.0f };
	guidLightRadius_ = { 1.0f,4.0f,2.0f };
	guidLightColor_[0] = { 0.85f,0.2f,0.24f };
	guidLightColor_[1] = { 0.72f,0.14f,0.21f };
	guidLightColor_[2] = { 0.39f,0.003f,0.14f };
	guidLightMoveCount_ = 0;
	guidLightMoveMaxCount_ = 5;

	heartLightColor_ = { 0.72f,0.14f,0.21f };
	heartLightIndensity_ = 1.0f;
	heartLightRadius_ = 10.0f;
	heartLightDecay_ = 6.0f;

	guidModel_.reset(Model::LoadFromOBJ("sphere", true));

	for (size_t i = 0; i < guidLightNum_ * 10; i++)
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

void GameLight::ImguiUpdate()
{
	ImGui::SetNextWindowSize(ImVec2(500, 100));
	ImGui::Begin("heartLight");

	ImGui::InputFloat("indensity", &heartLightIndensity_);
	ImGui::InputFloat("radius", &heartLightRadius_);
	ImGui::InputFloat2("decay", &heartLightDecay_);

	ImGui::End();
}

void GameLight::DrawForward3D(ViewProjection* view)
{
	for (size_t i = 0; i < guidLightNum_ * enmeyDeadNum_; i++)
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

void GameLight::GuidLightUpdate(std::vector<ItemPaper*> itemPapers, Enemy& enemy)
{
	guidLightStartNum_ = (int)(pointLightsInfo_.size() + 2);

	for (size_t i = 0; i < guidLightNum_; i++)
	{
		if (i == 0) {
			int firstNum = (int)(guidLightStartNum_ * enemy.GetDeadNum());
			// ポイントライトの情報を更新
			lightGroup_->SetPointLightColor(firstNum, heartLightColor_);
			lightGroup_->SetPointLightPos(firstNum, enemyDeadPos_);
			lightGroup_->SetPointLightIndensity(firstNum, heartLightIndensity_);
			lightGroup_->SetPointLightRadius(firstNum, heartLightRadius_);
			lightGroup_->SetPointLightDecay(firstNum, heartLightDecay_);
			lightGroup_->SetPointLightActive(firstNum, true);

		}
		else {
			int pointLightNum = (int)((guidLightStartNum_ * enemy.GetDeadNum()) + i);
			int guidNum = (int)((guidLightNum_ * (enemy.GetDeadNum() - 1)) + i);

			switch (guidLights_[guidNum]->state)
			{
			case ShiftTime:// 最初の生成のタイミングをずらすフェーズ
				guidLights_[guidNum]->startTimer++;
				guidLights_[guidNum]->guidlightObj->worldTransform_.translation = enemyDeadPos_;

				if (guidLights_[guidNum]->startTimer >= guidLights_[guidNum]->startTaiming) {
					guidLights_[guidNum]->startTimer = 0;
					guidLights_[guidNum]->state = SetValue;
				}
				break;
			case SetValue:// 各種移動等に必要な値を設定
			{
				guidLights_[guidNum]->guidlightObj->worldTransform_.translation = enemyDeadPos_;
				Vector3 vel;
				float length = 0;
				float minLength = 9999;

				// アイテムのなかで一番近いものの座標とベクトルの長さを抽出
				for (auto j : itemPapers)
				{
					if (j->GetIsCheckItem() == false) {
						vel = j->billTex_.worldTransform_.translation - enemyDeadPos_;
						length = vel.length();

						if (length < minLength) {
							guidLights_[guidNum]->startPos = enemyDeadPos_;
							guidLights_[guidNum]->endPos = j->billTex_.worldTransform_.translation;
							guidLights_[guidNum]->halfVel = vel / 2;
							guidLights_[guidNum]->easeGuid.SetEaseLimitTime(60);
							minLength = length;
						}
					}
				}
				guidLights_[guidNum]->state = Move;
				break;
			}
			case Move:// 移動処理
			{
				guidLights_[guidNum]->easeGuid.Update();

				if (guidLights_[guidNum]->IsShift == false)
				{
					guidLights_[guidNum]->IsShift = true;
					guidLights_[guidNum]->shiftVel.x = Random::RangeF(-3.0f, 3.0f);
					guidLights_[guidNum]->shiftVel.y = Random::RangeF(1.0f, 5.0f);
					guidLights_[guidNum]->shiftVel.z = Random::RangeF(-3.0f, 3.0f);

					guidLights_[guidNum]->halfVel += guidLights_[guidNum]->shiftVel;
					guidLights_[guidNum]->colPoint = guidLights_[guidNum]->startPos + guidLights_[guidNum]->halfVel;
				}

				// ベジエで移動
				Vector3 BeziersVel = guidLights_[guidNum]->easeGuid.LerpBezireQuadratic(guidLights_[guidNum]->startPos, guidLights_[guidNum]->colPoint, guidLights_[guidNum]->endPos);
				float lightIndensity = guidLights_[guidNum]->easeGuid.LerpBezireQuadratic(guidLightIdensity_.x, guidLightIdensity_.y, guidLightIdensity_.z);
				float lightRadius = guidLights_[guidNum]->easeGuid.LerpBezireQuadratic(guidLightRadius_.x, guidLightRadius_.y, guidLightRadius_.z);
				Vector3 lightColor = guidLights_[guidNum]->easeGuid.LerpBezireQuadratic(guidLightColor_[0], guidLightColor_[1], guidLightColor_[2]);
				guidLights_[guidNum]->guidlightObj->worldTransform_.translation = BeziersVel;
				guidLights_[guidNum]->guidlightObj->worldTransform_.color = Vector4(lightColor.x, lightColor.y, lightColor.z, 0.3f);

				// ポイントライトの情報を更新
				lightGroup_->SetPointLightColor(pointLightNum, lightColor);
				lightGroup_->SetPointLightPos(pointLightNum, BeziersVel);
				lightGroup_->SetPointLightIndensity(pointLightNum, lightIndensity);
				lightGroup_->SetPointLightRadius(pointLightNum, lightRadius);
				lightGroup_->SetPointLightDecay(pointLightNum, playerPointLightDecay_);
				lightGroup_->SetPointLightActive(pointLightNum, true);

				// 導きの光の最後尾のものが移動完了したらカウントをプラス
				if (guidLights_[(guidLightNum_ * enemy.GetDeadNum()) - 1]->easeGuid.GetIsEnd() == true)
				{
					enemy.AddLightMoveCount();
				}

				// ベジエが終了したらリセットをして再度始める準備をする
				if (guidLights_[guidNum]->easeGuid.GetIsEnd() == true) {
					guidLights_[guidNum]->easeGuid.Reset();
					guidLights_[guidNum]->state = ShiftTime;
					guidLights_[guidNum]->IsShift = false;

				}

				break;
			}
			}

			// オブジェクトの情報更新
			guidLights_[guidNum]->guidlightObj->Update();
		}

	}
}

void GameLight::EnemyGuidLightUpdate(std::vector<ItemPaper*> itemPapers)
{
	enmeyDeadNum_ = 0;

	// 敵のディゾルブ処理が終了していたら
	for (auto e : enemysInfo_)
	{

		if (e->GetDissolveEnd() == true) {
			enmeyDeadNum_++;
		}

		// 導きの光の処理
		if (e->GetDissolveEnd() == true && e->GetDeadMotionEnd() == false) {

			e->SetDeadNum(enmeyDeadNum_);
			enemyDeadPos_ = e->worldTransform_.translation;

			GuidLightUpdate(itemPapers, *e);
		}

		// 導きの光のガイド数が限界に達したら死亡モーションが終了とする
		if (e->GetDissolveEnd() == true &&
			e->GetLightMoveCount() >= guidLightMoveMaxCount_) {

			// モーション終了後にライトを消す
			GuidLightOffUpdate(*e);

			e->SetLightMoveEnd(true);
		}
	}

	// ライトグループの更新
	lightGroup_->Update();
}

void GameLight::GuidLightOffUpdate(Enemy& enemy)
{
	for (size_t i = 0; i < guidLightNum_; i++)
	{
		int pointLightNum = (int)((guidLightStartNum_ * enemy.GetDeadNum()) + i);
		int guidNum = (int)((guidLightNum_ * (enemy.GetDeadNum() - 1)) + i);
		guidLights_[guidNum]->state = ShiftTime;
		guidLights_[guidNum]->easeGuid.Reset();
		guidLights_[guidNum]->IsShift = false;
		lightGroup_->SetPointLightActive(pointLightNum, false);
		lightGroup_->SetPointLightPos(pointLightNum, lightOffPos_);
	}

	// ライトグループの更新
	lightGroup_->Update();
}
