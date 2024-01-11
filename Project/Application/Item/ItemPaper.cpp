#include "ItemPaper.h"
#include "Collision.h"
#include "InputManager.h"
#include "MathUtil.h"
Player* ItemPaper::sPlayer_ = nullptr;

void ItemPaper::Initialize(std::string fileName)
{
	spriteTex_.reset(TextureManager::Load2DTextureP(fileName));

	scaleBig_ = { 0.2f,0.2f,0.2f };
	scaleSmall_ = { 0.1f,0.1f,0.1f };

	billTex_.Initialize();
	billTex_.LoadTexture("itemEffect.png");
	billTex_.worldTransform_.translation.y = 0.6f;
	billTex_.worldTransform_.scale = scaleSmall_;

	itemSprite_.Initialize(spriteTex_.get());
	itemSprite_.SetPosition({ WinApp::window_width / 2,WinApp::window_height / 2 });

	// 状態の初期化
	colState_ = withInRange;
	itemState_ = sparkling;

	alphaChangeValue_ = 0.02f;

	IsBig_ = false;
	IsCheckSprite_ = false;
	IsGetItem_ = false;
	IsKeyItem_ = false;

	// 表示するためのタイマー
	displayTimer_ = 0;
	displayTimeLimit_ = 5;

	easeScale_ = { 0.7f,0.7f,0.7f };
}

void ItemPaper::Update()
{
	// 当たり判定の更新処理
	OncolToPlayerUpdate();

	// 状態の更新処理
	ColStateUpdate();

	// ビルボードの情報を更新など
	billTex_.Update();
}

void ItemPaper::UpdateWorldMatrix()
{
	billTex_.worldTransform_.UpdateMatrix();
}

void ItemPaper::Draw3D()
{
	// もしアイテムの状態がキラキラの時
	if(itemState_==sparkling){
		billTex_.Draw();
	}
	
}

void ItemPaper::Draw2D()
{
	// もしアイテムの状態が拾った後の時
	// アイテムの確認がとれていないなら描画をする
	if (itemState_ == afterPickItUp) {
		if (IsKeyItem_ == true) {
			itemSprite_.Draw();
		}
		else if (IsCheckSprite_ == false) {
			itemSprite_.Draw();
		}
	}
}

void ItemPaper::Reset()
{
	colState_ = withInRange;
	itemState_ = sparkling;

	easeTrans_.Reset();
	easeKeyItem_.Reset();

	IsBig_ = false;
	IsCheckSprite_ = false;
	IsGetItem_ = false;
	IsEaseKeyItem_ = false;

	displayTimer_ = 0;

	itemSprite_.SetPosition({ WinApp::window_width / 2,WinApp::window_height / 2 });
	itemSprite_.SetScale({ 1,1,1 });
}

void ItemPaper::SetWorldTransformPos(Vector3 wTranslation)
{
	billTex_.worldTransform_.translation = wTranslation;

}

void ItemPaper::SetIsKeyItem(bool isKeyItem)
{
	IsKeyItem_ = isKeyItem;
}

void ItemPaper::SetIsEaseKey(bool isEaseKey)
{
	IsEaseKeyItem_ = isEaseKey;
}

void ItemPaper::SetEaseKeyPos(Vector2 easeKeyPos)
{
	easeKeyPos_ = easeKeyPos;
}

void ItemPaper::OncolToPlayerUpdate()
{
	// プレイヤーの情報をスフィアのものに登録
	Sphere pcol;
	pcol.center = sPlayer_->worldTransform_.translation;
	pcol.center.y += 1.0f;
	pcol.radius = 1.0f;

	// アイテムビルボードオブジェの情報をスフィアに設定
	Sphere sItemRange;
	sItemRange.center = billTex_.worldTransform_.translation;
	sItemRange.radius = 6.0f;

	Sphere sItemInRange;
	sItemInRange.center = billTex_.worldTransform_.translation;
	sItemInRange.radius = 1.5f;

	// アイテムのキラキラが表示される範囲だったら
	if (Collision::CheckSphere2Sphere(pcol, sItemRange)) {

		colState_ = withInRange;

		// アイテムが取得できる範囲だったら
		if (Collision::CheckSphere2Sphere(pcol, sItemInRange)) {

			// Aボタンを押したらアイテムを表示し、アイテム取得フラグをオン
			if (JoypadInput::GetButtonDown(PadCode::ButtonA)) {
				itemState_ = afterPickItUp;
				IsGetItem_ = true;
			}
		}
	}
	else {
		colState_ = outOfRange;
	}
}

void ItemPaper::ColStateUpdate()
{
	switch (colState_)
	{
	case ItemPaper::withInRange:// 範囲内
		if (billTex_.worldTransform_.color.w <= 1.0f) {
			billTex_.worldTransform_.color.w += alphaChangeValue_;
		}
		// アイテムの状態の更新処理
		ItemStateUpdate();

		break;
	case ItemPaper::outOfRange:// 範囲外
		if (billTex_.worldTransform_.color.w >= 0.0f) {
			billTex_.worldTransform_.color.w -= alphaChangeValue_;
		}

		break;
	default:
		break;
	}
}

void ItemPaper::ItemStateUpdate()
{
	switch (itemState_)
	{
	case ItemPaper::sparkling:// キラキラの状態
		billTex_.worldTransform_.rotation.z -= MathUtil::DegreeToRadian(3);

		// 回転の情報が360度超えたら0に戻す
		if (billTex_.worldTransform_.rotation.z <= MathUtil::DegreeToRadian(-360)) {
			billTex_.worldTransform_.rotation.z = 0;
		}

		// スケールが小さいとき
		if (IsBig_ == false) {
			easeTrans_.SetEaseLimitTime(60);
			easeTrans_.Update();

			billTex_.worldTransform_.scale = easeTrans_.easeInCircVec3(scaleSmall_, scaleBig_);

			// もしイージングが終了したら
			if (easeTrans_.GetIsEnd() == true) {
				easeTrans_.Reset();
				IsBig_ = true;
			}
		}
		// スケールが大きいとき
		if (IsBig_ == true) {
			easeTrans_.SetEaseLimitTime(60);
			easeTrans_.Update();

			billTex_.worldTransform_.scale = easeTrans_.easeOutCircVec3(scaleBig_, scaleSmall_);

			// もしイージングが終了したら
			if (easeTrans_.GetIsEnd() == true) {
				easeTrans_.Reset();
				IsBig_ = false;
			}
		}

		break;
	case ItemPaper::afterPickItUp:// 拾った後の状態

		// もしタイマーがリミット以下なら
		// タイマーをプラスして、スイッチ文を抜ける
		if (displayTimer_ < displayTimeLimit_) {
			displayTimer_++;
			break;
		}
		// Aボタンを押したらアイテムの確認がとれたことになる
		if (JoypadInput::GetButtonDown(PadCode::ButtonA)) {
			IsCheckSprite_ = true;
			IsGetItem_ = false;
		}
		break;
	default:
		break;
	}

}

void ItemPaper::KeyItemEaseUpdate()
{
	easeKeyItem_.SetEaseLimitTime(60);
	easeKeyItem_.Update();

	Vector2 windowHalfSize = { WinApp::window_width / 2,WinApp::window_height / 2 };
	Vector3 defuScale = { 1,1,1 };

	itemSprite_.SetPosition(easeKeyItem_.easeInCircVec2(windowHalfSize, easeKeyPos_));
	itemSprite_.SetScale(easeKeyItem_.easeInCircVec3(defuScale, easeScale_));
}
