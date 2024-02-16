#include "Player.h"
#include "Input.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "JoyPadInput.h"
#include "MathUtil.h"
#include "Random.h"
#include "ItemPaper.h"

Player* Player::Create(Model* model)
{
	// 3Dオブジェクトのインスタンスを生成
	Player* instance = new Player();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
	}

	// モデルのセット
	if (model) {
		instance->SetModel(model);
	}
	return instance;
}

Player::~Player()
{


}

bool Player::Initialize()
{
	if (!Object3d::Initialize()) {
		return false;
	}
	worldTransform_.translation.x -= 1;
	worldTransform_.translation.z -= 32;

	frontW_.Initialize();
	frontW_.translation = { 0,0,1 };
	frontW_.parent_ = &worldTransform_;

	bulletModel_.reset(Model::LoadFromOBJ("sphere", true));
	cameraWorld_.Initialize();

	// プレイヤーのHPの初期化
	playerHitPoint_ = playerHitPointMax_;

	// プレイヤーのテクスチャハンドルの初期化
	textureHandleHpBar_.reset(TextureManager::Load2DTextureP("hpBar.png"));
	textureHandleHpInside_.reset(TextureManager::Load2DTextureP("hpInside.png"));

	// プレイヤーのスプライトの初期化
	playerHpBar_ = std::make_unique<Sprite>();
	playerHpBar_->Initialize(textureHandleHpBar_.get());
	playerHpBar_->SetAnchorPoint({ 0,0 });

	playerHpInside_ = std::make_unique<Sprite>();
	playerHpInside_->Initialize(textureHandleHpInside_.get());
	playerHpInside_->SetAnchorPoint({ 0,0 });

	questionTex_.reset(TextureManager::Load2DTextureP("question.png"));
	surprisedTex_.reset(TextureManager::Load2DTextureP("surprised.png"));

	questionBillTex_.Initialize();
	questionBillTex_.SetTexture(questionTex_.get());
	questionBillTex_.worldTransform_.translation = worldTransform_.translation;
	questionBillTex_.worldTransform_.translation.y += 2;
	questionBillTex_.worldTransform_.scale = { 0.1f,0.1f,0.1f };

	return true;
}

void Player::InitializeSearchRing()
{
	searchRingModel_.reset(Model::LoadFromOBJ("searchRing", true));

	searchRingObj_.Initialize();
	searchRingObj_.SetModel(searchRingModel_.get());

	searchRingObj_.worldTransform_.color.w = 0.7f;

	ringModel_.reset(Model::LoadFromOBJ("ring", true));

	ringObj_.Initialize();
	ringObj_.SetModel(ringModel_.get());

	ringState_ = ValueSet;
}

void Player::gameSceneFirstUpdate()
{
	if (IsEndTurnAround_ == false) {
		switch (firstEventState_)
		{
		case FirstMove:// 最初の移動
			easeQuestion_.SetEaseLimitTime(80);
			easeQuestion_.Update();
			worldTransform_.translation.z = easeQuestion_.Lerp(firstEventMoveZS, firstEventMoveZE);

			if (easeQuestion_.GetIsEnd() == true) {
				easeQuestion_.Reset();
				firstEventState_ = InterTurn;
			}

			break;

		case InterTurn:
			// イージングの処理
			easeQuestion_.SetEaseLimitTime(20);
			easeQuestion_.Update();
			worldTransform_.rotation.y = MathUtil::DegreeToRadian(easeQuestion_.Lerp(0, easeQ_Rota2));
			questionBillTex_.worldTransform_.rotation.z = MathUtil::DegreeToRadian(easeQuestion_.Lerp(0, easeQ_Rota2));
			if (easeQuestion_.GetIsEnd() == true) {
				easeQuestion_.Reset();
				firstEventState_ = FirstTurn;
			}

			break;
		case FirstTurn:// 一回目の回転

			// イージングの処理
			easeQuestion_.SetEaseLimitTime(40);
			easeQuestion_.Update();
			worldTransform_.rotation.y = MathUtil::DegreeToRadian(easeQuestion_.Lerp(easeQ_Rota2, easeQ_Rota1));
			questionBillTex_.worldTransform_.rotation.z = MathUtil::DegreeToRadian(easeQuestion_.Lerp(easeQ_Rota2, easeQ_Rota1));

			if (easeQuestion_.GetIsEnd() == true) {
				aroundStopTimer++;
			}
			if (aroundStopTimer >= aroundStopTimeLimit) {
				aroundStopTimer = 0;
				aroundStopTimeLimit = Random::Range(20, 40);
				easeQuestion_.Reset();
				firstEventState_ = SecondTurn;
			}
			break;
		case SecondTurn:// 二回目の回転

			// イージングの処理
			easeQuestion_.SetEaseLimitTime(30);
			easeQuestion_.Update();
			worldTransform_.rotation.y = MathUtil::DegreeToRadian(easeQuestion_.Lerp(easeQ_Rota1, easeQ_Rota2));
			questionBillTex_.worldTransform_.rotation.z = MathUtil::DegreeToRadian(easeQuestion_.Lerp(easeQ_Rota1, easeQ_Rota2));

			if (easeQuestion_.GetIsEnd() == true) {
				aroundStopTimer++;
			}
			if (aroundStopTimer >= aroundStopTimeLimit) {
				aroundStopTimer = 0;
				aroundStopTimeLimit = Random::Range(20, 40);
				easeQuestion_.Reset();
				firstEventState_ = FirstTurn;
				turnAroundCount_++;
			}
			break;

		case Surprised:// びっくりのフェーズ
			// イージングの処理
			easeQuestion_.SetEaseLimitTime(30);
			easeQuestion_.Update();
			worldTransform_.rotation.y = MathUtil::DegreeToRadian(easeQuestion_.Lerp(easeQ_Rota2, easeSRota));

			if (easeQuestion_.GetIsEnd() == true) {
				easeQuestion_.Reset();
				IsEndTurnAround_ = true;
			}
			break;
		default:
			break;
		}
	}

	// 回転が２カウント以上になったら
	if (turnAroundCount_ >= 2) {
		firstEventState_ = Surprised;
	}

	frontW_.UpdateMatrix();
}

void Player::Update()
{
	// 生きている間の更新処理
	if (IsAlive_) {
		// 前フレームの座標を代入
		worldTransform_.oldTranslation = worldTransform_.translation;

		// ゲームシーンの最初のきょろきょろ更新処理
		gameSceneFirstUpdate();

		// ビルボードの更新処理
		questionBillTex_.worldTransform_.translation = worldTransform_.translation;
		questionBillTex_.worldTransform_.translation.y += 2;
		questionBillTex_.Update();

		if (IsEndTurnAround_ == true) {

			if (IsCanMove_ == true) {
				// 移動の更新処理
				MoveUpdate();
			}

			// スプライトのポジションやサイズの変更
			playerHpBar_->SetPosition(pHpBarPos_);
			playerHpInside_->SetPosition(pHpInsidePos_);
			playerHpBar_->SetSize(pHpBarSize_);
			playerHpInside_->SetSize(pHpInsideSize_);

			// 攻撃関数
			//Attack();

			// プレイヤーのHPが０以下ならプレイヤーが死ぬ
			if (playerHitPoint_ <= (float)ZERO) {
				IsAlive_ = false;
			}
		}
	}

	//弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}

	// 行列の更新など
	Object3d::Update();
}

void Player::Draw(ViewProjection* view)
{
	// シルエットの描画
	Object3d::SetBlendMode(BlendMode::SILHOUETTE);
	Object3d::Draw(view);

	// オブジェクト本体の描画
	Object3d::SetBlendMode(BlendMode::NORMAL);
	Object3d::Draw(view);

	//弾描画
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Draw(view);
	}
}

void Player::DrawRing(ViewProjection* view)
{
	// サーチリングの描画
	searchRingObj_.Draw(view);

	// リングの描画
	ringObj_.Draw(view);
}

void Player::DrawBillTex()
{
	// ビルボード描画
	if (IsItemInRange_ == true) {
		questionBillTex_.worldTransform_.rotation.z = 0;
		questionBillTex_.SetTexture(surprisedTex_.get());
		questionBillTex_.Draw();
	}
	else if (firstEventState_ == InterTurn ||
			 firstEventState_ == FirstTurn ||
			 firstEventState_ == SecondTurn) 
	{
		questionBillTex_.SetTexture(questionTex_.get());
		questionBillTex_.Draw();
	}
	else if (IsItemRange_ == true) {
		questionBillTex_.worldTransform_.rotation.z = 0;
		questionBillTex_.SetTexture(questionTex_.get());
		questionBillTex_.Draw();
	}
}

void Player::SetIsCanMove(bool isCamMove)
{
	IsCanMove_ = isCamMove;
}

void Player::OnCollision()
{

}

const Vector3 Player::GetWorldPosition() const
{
	//ワールド座標を入れる変数
	Vector3 worldPos;
	//ワールド行列移動成分を取得(ワールド座標)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

const WorldTransform* Player::GetCameraWorld() const
{
	return &cameraWorld_;
}

const Vector3 Player::GetFrontPos() const
{
	return Vector3(frontW_.matWorld_.m[3][0], frontW_.matWorld_.m[3][1], frontW_.matWorld_.m[3][2]);
}

const Vector3 Player::GetFrontVec() const
{
	Vector3 velocity;
	Vector3 frontVec = { frontW_.matWorld_.m[3][0],frontW_.matWorld_.m[3][1] ,frontW_.matWorld_.m[3][2] };


	// 自機から照準オブジェクトへのベクトル
	velocity = frontVec - worldTransform_.translation;
	velocity.normalize();

	return velocity;
}

const bool Player::GetIsAlive() const
{
	return IsAlive_;
}

void Player::SetWorldTransInfo(WorldTransform worldTrans)
{
	worldTransform_.translation = worldTrans.translation;
	worldTransform_.scale = worldTrans.scale;
	worldTransform_.rotation = worldTrans.rotation;
}

void Player::SetGameCamera(GameCamera* gameCamera)
{
	bGameCamera = gameCamera;
}

void Player::SetIsItemRange(bool isItemRange)
{
	IsItemRange_ = isItemRange;
}

void Player::SetIsItemInRange(bool isItemInRange)
{
	IsItemInRange_ = isItemInRange;
}

std::list<std::unique_ptr<PlayerBullet>>& Player::GetBullets()
{
	return bullets_;
}

const bool Player::GetIsEndTurnAround() const
{
	return IsEndTurnAround_;
}

void Player::pushBackOnCol()
{
	const Vector3 up = { 0,1,0 };
	Vector3 info_ = rejectVec;
	Vector3 rejectDir = info_.normalize();
	float cos = Vector3::dot(rejectDir, up);

	Vector3 move;

	// 地面判定しきい値
	const float threshold = cosf(Vector3::Deg2Rad(50.0f));

	if (-threshold < cos && cos < threshold) {
		//sphere->center += info.reject;

	}
	move += rejectVec;
	worldTransform_.translation += move;
	worldTransform_.UpdateMatrix();
}

void Player::OnColDownSpeed()
{
	float downS = 2;
	moveSpeed_ /= downS;
	if (moveSpeed_ <= moveSpeedMin_) {
		moveSpeed_ = moveSpeedMin_;
	}
}

void Player::OnColUpSpeed()
{
	float upS = 0.005f;
	moveSpeed_ += upS;
	if (moveSpeed_ >= moveSpeedMax_) {
		moveSpeed_ = moveSpeedMax_;
	}
}

void Player::Draw2DFront()
{

	playerHpInside_->Draw();
	playerHpBar_->Draw();
}

void Player::DrawImgui()
{
	// 表示項目の追加--------//
	ImGui::Begin("Player");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));

	ImGui::InputFloat("HitPoint", &playerHitPoint_, 0, playerHitPointMax_);

	ImGui::End();

	ImGui::Begin("playerSprite");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));

	ImGui::InputFloat2("hpBarPos", &pHpBarPos_.x, "%.2f");
	ImGui::InputFloat2("hpInsidePos", &pHpInsidePos_.x, "%.2f");
	ImGui::InputFloat2("hpBarSize", &pHpBarSize_.x, "%.2f");
	ImGui::InputFloat2("hpInsideSize", &pHpInsideSize_.x, "%.2f");

	ImGui::End();
}

void Player::Reset()
{
	// ワールド変換データのリセット
	worldTransform_.translation = { -1,ZERO,firstEventMoveZS };
	worldTransform_.rotation = { ZERO,ZERO,ZERO };

	// プレイヤーのHPのリセット
	playerHitPoint_ = playerHitPointMax_;

	// プレイヤーの状態を復活
	IsAlive_ = true;

	// プレイヤーのHPスプライトの情報をリセット
	pHpInsidePos_ = { 20,50 };
	pHpInsideSize_ = { 500,70 };

	// 最初の処理のリセット
	IsEndTurnAround_ = false;
	firstEventState_ = FirstMove;
	easeQuestion_.Reset();
	turnAroundCount_ = 0;
	aroundStopTimer = 0;
	IsCanMove_ = false;
}

void Player::MoveUpdate()
{
	Matrix4 mathMat;

	// 移動ベクトルをY軸周りの角度で回転

	Vector3 vectorX = { 0.01f,ZERO,ZERO };
	vectorX = MathUtil::MatVector(worldTransform_.matWorld_, vectorX);
	vectorX.normalize();
	Vector3 vectorZ = { ZERO,ZERO,-0.01f };
	vectorZ = MathUtil::MatVector(worldTransform_.matWorld_, vectorZ);
	vectorZ.normalize();

	vectorX *= moveValue;
	vectorZ *= moveValue;

	Vector3 move = { ZERO,ZERO,ZERO };
	Vector3 rot = { ZERO,ZERO,ZERO };

	Vector2 joyStickInfoL = { ZERO,ZERO };
	Vector2 joyStickInfoR = { ZERO,ZERO };

	// 向いてる方向に移動
	moveVel_ = { ZERO,ZERO,ZERO };
	frontVec_ = { ZERO,ZERO,ZERO };

	// カメラの前ベクトル
	Vector3 cameForward = worldTransform_.translation - bGameCamera->GetView().eye;
	cameForward.y = ZERO;

	// カメラの右ベクトル
	Vector3 up = { ZERO, 1,ZERO };
	Vector3 cameRight = Vector3::Cross(cameForward, up);

	Vector3 stick =
	{
		Pad::GetStick(PadCode::LeftStick, 300).x,
		ZERO,
		Pad::GetStick(PadCode::LeftStick, 300).y,
	};

	// スティック入力がゼロでは無いなら
	if (stick != ZERO)
	{
		Vector3 stickMoveVec = { ZERO,ZERO,ZERO };

		stickMoveVec.x = stick.normalize().x;
		stickMoveVec.z = stick.normalize().z;

		frontVec_ = cameForward * -stickMoveVec.z + cameRight * -stickMoveVec.x;
	}

	// 前ベクトルがゼロではないなら
	if (frontVec_ != ZERO)
	{
		if (IsAttack_ == true) {
			moveSpeed_ = 0.08f;
			moveVel_ = frontVec_.normalize() * moveSpeed_;

		}
		else if (IsAttack_ == false) {
			OnColUpSpeed();
			moveVel_ = frontVec_.normalize() * moveSpeed_;
			worldTransform_.rotation.y = atan2f(frontVec_.x, frontVec_.z);
		}

		// 移動ベクトル方向に移動
		worldTransform_.translation += moveVel_;
	}

	// カメラの回転用のものにかりにコピー
	cameraWorld_.translation = worldTransform_.translation;
	cameraWorld_.rotation.y += MathUtil::DegreeToRadian(rot.y);
	cameraWorld_.UpdateMatrix();


	ImGui::Begin("joyPadInfo");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));

	ImGui::InputFloat2("joySrickL", &joyStickInfoL.x, "%.2f");
	ImGui::InputFloat2("joySrickR", &joyStickInfoR.x, "%.2f");
	ImGui::InputFloat3("playerRot", &worldTransform_.rotation.x, "%.2f");

	ImGui::End();


	frontW_.UpdateMatrix();
	// ワールド行列更新
	UpdateWorldMatrix();
}

void Player::Attack()
{
	//デスフラグの立った弾を削除
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) { return bullet->IsDead(); });
	bulletInterTimer--;
	if (JoypadInput::GetButton(PadCode::ButtonA))//input_->PushKey(DIK_Y)) {
	{
		IsAttack_ = true;

		if (bulletInterTimer <= 0) {

			//弾の速度
			const float kBulletSpeed = 0.3f;
			Vector3 velocity;
			Vector3 frontVec = { frontW_.matWorld_.m[3][0],frontW_.matWorld_.m[3][1] ,frontW_.matWorld_.m[3][2] };


			// 自機から照準オブジェクトへのベクトル
			velocity = frontVec - worldTransform_.translation;
			velocity.normalize();
			velocity *= kBulletSpeed;

			//速度ベクトルを自機の向きに合わせて回転させる
			// velocity = velocity * worldTransform_.matWorld_;

			//弾を生成し、初期化
			std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
			newBullet->Initialize(bulletModel_.get(), worldTransform_, velocity);

			//弾を登録する
			bullets_.push_back(std::move(newBullet));

			bulletInterTimer = bulletInterval;
		}

	}
	else {
		IsAttack_ = false;
	}
}

void Player::SearchRingUpdate(std::vector<ItemPaper*>itemPapers)
{
	Vector3 vel;
	float length = 0;

	Vector3 minVel;
	float minLength = 9999;
	float angle;

	// アイテムのなかで一番近いものの座標とベクトルの長さを抽出
	for (auto i: itemPapers)
	{
		if (i->GetIsCheckItem() == false) {
			vel = i->billTex_.worldTransform_.translation - worldTransform_.translation;
			length = vel.length();

			if (length < minLength) {
				minVel = vel;
				minLength = length;
			}
		}
	}

	// サーチリングのY回転を設定
	angle = atan2(minVel.x, minVel.z);
	searchRingObj_.worldTransform_.rotation.y = angle;

	// プレイヤーの座標と同じものにする
	searchRingObj_.worldTransform_.translation = worldTransform_.translation;
	ringObj_.worldTransform_.translation = worldTransform_.translation;

	// リングの状態更新処理
	switch (ringState_)
	{
	case Player::ValueSet:
		ringObj_.worldTransform_.scale = defuRingScale_;
		ringObj_.worldTransform_.color.w = defuRingAlpha_;
		ringState_ = EaseRing;
		break;
	case Player::EaseRing:
		easeRing_.SetEaseLimitTime(80);
		easeRing_.Update();

		ringObj_.worldTransform_.color.w = easeRing_.easeOutCubic(defuRingAlpha_, bigRingAlpha_);
		ringObj_.worldTransform_.scale = easeRing_.easeOutCircVec3(defuRingScale_, bigRingScale_);

		if (easeRing_.GetIsEnd() == true) {
			easeRing_.Reset();
			ringState_ = ValueSet;
		}

		break;
	default:
		break;
	}

	// サーチリングを更新
	searchRingObj_.Update();
	ringObj_.Update();
}

void Player::OnColHitPoint()
{
	playerHitPoint_ -= 2;
	// HPのセット
	float nowHp = playerHitPoint_ / playerHitPointMax_;
	if (nowHp <= 0.05f) {
		pHpInsideSize_.x = 500.0f * nowHp;
		pHpInsideSize_.y -= 4.7f;
		pHpInsidePos_.y += 1.f;
	}
	else if (nowHp <= 0.1f) {
		pHpInsideSize_.x = 500.0f * nowHp;
		pHpInsideSize_.y -= 2.5f;
		pHpInsidePos_.y += 1.f;
	}
	else {
		pHpInsideSize_ = { 500.0f * nowHp,70.0f };
	}

	// HPのポジションのずらし
	float hpMovePos = 0;
	hpMovePos = (1 - (nowHp)) * 100;
	pHpInsidePos_.x = (20 + (hpMovePos * 0.3f));

	playerHpInside_->SetPosition(pHpInsidePos_);
	playerHpInside_->SetSize(pHpInsideSize_);
}
