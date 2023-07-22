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
	//worldTransform_.position_.y += 100;
	// 半径分だけ足元から浮いた座標を球の中心にする
	//SetCollider(new SphereCollider(Vector3(0, radius, 0), radius));
	//collider_->SetAttribute(COLLISION_ATTR_ALLIES);

	return true;
}

void Player::Update()
{

	MoveUpdate();

	// 行列の更新など
	Object3d::Update();
}

void Player::Draw(ViewProjection* view)
{
	Object3d::Draw(view);
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
	//Matrix4 matRot;
	//matRot.rotateY(worldTransform_.rotation.y);
	//move = matRot.transformNotW(move, matRot);

	// 向いてる方向に移動

	// Lスティックで移動
	if (JoypadInput::GetStick(PadCode::LeftStick).x > deadZone ||
		JoypadInput::GetStick(PadCode::LeftStick).x < -deadZone ||
		JoypadInput::GetStick(PadCode::LeftStick).y > deadZone ||
		JoypadInput::GetStick(PadCode::LeftStick).y < -deadZone) {

		move.x += JoypadInput::GetStick(PadCode::LeftStick).x / 1000 * vectorX.x;
		move.z += JoypadInput::GetStick(PadCode::LeftStick).x / 1000 * vectorX.z;
		move.x += JoypadInput::GetStick(PadCode::LeftStick).y / 1000 * vectorZ.x;
		move.z += JoypadInput::GetStick(PadCode::LeftStick).y / 1000 * vectorZ.z;

		joyStickInfoL.x = JoypadInput::GetStick(PadCode::LeftStick).x;
		joyStickInfoL.y = JoypadInput::GetStick(PadCode::LeftStick).y;

	}

	// Rスティックでカメラ回転
	if (JoypadInput::GetStick(PadCode::RightStick).x > deadZone ||
		JoypadInput::GetStick(PadCode::RightStick).x < -deadZone) {
		rot.y = JoypadInput::GetStick(PadCode::RightStick).x / 1000 * 2;

		joyStickInfoR.y = JoypadInput::GetStick(PadCode::RightStick).x / 1000;
	}

	worldTransform_.rotation.y += MathUtil::DegreeToRadian(rot.y);
	worldTransform_.translation += move;
	

	ImGui::Begin("joyPadInfo");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));

	ImGui::InputFloat2("joySrickL", &joyStickInfoL.x, "%.2f");
	ImGui::InputFloat2("joySrickR", &joyStickInfoR.x, "%.2f");
	ImGui::InputFloat3("playerPos", &worldTransform_.translation.x, "%.2f");

	ImGui::End();


	// ワールド行列更新
	UpdateWorldMatrix();
}
