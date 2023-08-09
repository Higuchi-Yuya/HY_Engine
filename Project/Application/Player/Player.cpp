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
	//input->Initialize();
	// コライダーの追加
	float radius = 0.6f;

	frontW_.Initialize();
	frontW_.translation = { 0,0,1 };
	frontW_.parent_ = &worldTransform_;

	bulletModel_.reset(Model::LoadFromOBJ("doragon"));
	cameraWorld_.Initialize();
	return true;
}

void Player::Update()
{
	// 移動の更新処理
	MoveUpdate();

	// 攻撃関数
	Attack();

	//弾更新
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}

	// 行列の更新など
	Object3d::Update();
}

void Player::Draw(ViewProjection* view)
{
	Object3d::Draw(view);
	//弾描画
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Draw(view);
	}
	//atari->Draw(view);
}

void Player::OnCollision(const CollisionInfo& info)
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

const std::list<std::unique_ptr<PlayerBullet>>& Player::GetBullets() 
{
	return bullets_;
}



void Player::MoveUpdate()
{
	Matrix4 mathMat;

	// 移動ベクトルをY軸周りの角度で回転

	Vector3 vectorX = { 0.01f,0,0 };
	vectorX = MathUtil::MatVector(worldTransform_.matWorld_,vectorX);
	vectorX.normalize();
	Vector3 vectorZ = { 0,0,-0.01f };
	vectorZ = MathUtil::MatVector(worldTransform_.matWorld_, vectorZ);
	vectorZ.normalize();

	vectorX *= moveValue;
	vectorZ *= moveValue;

	Vector3 move = { 0,0,0 };
	Vector3 rot = { 0,0,0 };

	Vector2 joyStickInfoL = { 0,0 };
	Vector2 joyStickInfoR = { 0,0 };

	// 向いてる方向に移動

	moveVel_ = { 0,0,0 };
	frontVec_ = { 0,0,0 };
	// カメラの前ベクトル
	Vector3 cameForward = worldTransform_.translation - bGameCamera->GetView().eye;
	cameForward.y = 0.0f;

	// カメラの右ベクトル
	Vector3 up = { 0, 1, 0 };
	Vector3 cameRight = Vector3::Cross(cameForward, up);

	Vector3 stick =
	{
		Pad::GetStick(PadCode::LeftStick, 300).x,
		0,
		Pad::GetStick(PadCode::LeftStick, 300).y,
	};
	if (stick != 0.0f)
	{
		Vector3 stickMoveVec = {0,0,0};

		stickMoveVec.x = stick.normalize().x;
		stickMoveVec.z = stick.normalize().z;

		frontVec_ = cameForward * -stickMoveVec.z + cameRight * -stickMoveVec.x;
	}

	if (frontVec_ != 0.0f)
	{
		moveVel_ = frontVec_.normalize() * moveSpeed_;

		worldTransform_.translation += moveVel_;
		worldTransform_.rotation.y = atan2f(frontVec_.x, frontVec_.z);
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
	ImGui::InputFloat3("playerPos", &worldTransform_.translation.x, "%.2f");

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
}
