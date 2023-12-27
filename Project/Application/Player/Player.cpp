#include "Player.h"
#include "Input.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "JoyPadInput.h"


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


	frontW_.Initialize();
	frontW_.translation = { 0,0,1 };
	frontW_.parent_ = &worldTransform_;

	bulletModel_.reset(Model::LoadFromOBJ("sphere",true));
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

	return true;
}

void Player::InitializeFlashLightRange()
{
	flashLightRangeModel_.reset(Model::LoadFromOBJ("flashlightRange", true));

	flashLightRangeObj_.Initialize();
	flashLightRangeObj_.SetModel(flashLightRangeModel_.get());
	
	flashLightRangeObj_.worldTransform_.scale = { 2.5f,1.5f,3.0f };

	flashLightRangeObj_.worldTransform_.translation = { 0,0,3.3f };
	flashLightRangeObj_.worldTransform_.parent_ = &worldTransform_;
}

void Player::Update()
{
	// 生きている間の更新処理
	if (IsAlive_){
		// 前フレームの座標を代入
		worldTransform_.oldTranslation = worldTransform_.translation;

		// 移動の更新処理
		MoveUpdate();

		// 攻撃関数
		Attack();

		// プレイヤーのHPが０以下ならプレイヤーが死ぬ
		if (playerHitPoint_ <= (float)ZERO) {
			IsAlive_ = false;
		}
	}
	
	//弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}

	// 行列の更新など
	Object3d::Update();

	// 懐中電灯の範囲のオブジェの更新処理
	flashLightRangeObj_.Update();
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

std::list<std::unique_ptr<PlayerBullet>>& Player::GetBullets()
{
	return bullets_;
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
	ImGui::InputFloat3("flashObjRange scale", &flashLightRangeObj_.worldTransform_.scale.x, 0, 5);
	ImGui::InputFloat3("flashObjRange pos", &flashLightRangeObj_.worldTransform_.translation.x, 0, 5);

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

void Player::DrawFlashLightRange(ViewProjection* view)
{
	flashLightRangeObj_.Draw(view);
}

void Player::Reset()
{
	// ワールド変換データのリセット
	worldTransform_.translation = { ZERO,ZERO,ZERO };
	worldTransform_.rotation = { ZERO,ZERO,ZERO };

	// プレイヤーのHPのリセット
	playerHitPoint_ = playerHitPointMax_;

	// プレイヤーの状態を復活
	IsAlive_ = true;

	// プレイヤーのHPスプライトの情報をリセット
	pHpInsidePos_ = { 20,50 };
	pHpInsideSize_ = { 500,70 };
}

void Player::MoveUpdate()
{
	Matrix4 mathMat;

	// 移動ベクトルをY軸周りの角度で回転

	Vector3 vectorX = { 0.01f,ZERO,ZERO };
	vectorX = MathUtil::MatVector(worldTransform_.matWorld_,vectorX);
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

	// スプライトのポジションやサイズの変更
	playerHpBar_->SetPosition(pHpBarPos_);
	playerHpInside_->SetPosition(pHpInsidePos_);
	playerHpBar_->SetSize(pHpBarSize_);
	playerHpInside_->SetSize(pHpInsideSize_);


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
