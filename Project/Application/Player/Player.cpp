#include "Player.h"
#include "Input.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "JoyPadInput.h"

Player* Player::Create(Model* model)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Player* instance = new Player();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
	}

	// ���f���̃Z�b�g
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
	// �R���C�_�[�̒ǉ�
	float radius = 0.6f;
	//worldTransform_.position_.y += 100;
	// ���a�������������畂�������W�����̒��S�ɂ���
	//SetCollider(new SphereCollider(Vector3(0, radius, 0), radius));
	//collider_->SetAttribute(COLLISION_ATTR_ALLIES);
	cameraWorld_.Initialize();
	return true;
}

void Player::Update()
{

	MoveUpdate();

	// �s��̍X�V�Ȃ�
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
	//���[���h���W������ϐ�
	Vector3 worldPos;
	//���[���h�s��ړ��������擾(���[���h���W)
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



void Player::MoveUpdate()
{
	Matrix4 mathMat;

	// �ړ��x�N�g����Y������̊p�x�ŉ�]

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

	// �����Ă�����Ɉړ�

	//// L�X�e�B�b�N�ňړ�
	//if (JoypadInput::GetStick(PadCode::LeftStick).x > deadZone ||
	//	JoypadInput::GetStick(PadCode::LeftStick).x < -deadZone ||
	//	JoypadInput::GetStick(PadCode::LeftStick).y > deadZone ||
	//	JoypadInput::GetStick(PadCode::LeftStick).y < -deadZone) {

	//	move.x += JoypadInput::GetStick(PadCode::LeftStick).x / 1000 * vectorX.x;
	//	move.z += JoypadInput::GetStick(PadCode::LeftStick).x / 1000 * vectorX.z;
	//	move.x += JoypadInput::GetStick(PadCode::LeftStick).y / 1000 * vectorZ.x;
	//	move.z += JoypadInput::GetStick(PadCode::LeftStick).y / 1000 * vectorZ.z;

	//	joyStickInfoL.x = JoypadInput::GetStick(PadCode::LeftStick).x;
	//	joyStickInfoL.y = JoypadInput::GetStick(PadCode::LeftStick).y;

	//}
	moveVel_ = { 0,0,0 };
	frontVec_ = { 0,0,0 };
	// �J�����̑O�x�N�g��
	Vector3 cameForward = worldTransform_.translation - bGameCamera->GetView().eye;
	cameForward.y = 0.0f;

	// �J�����̉E�x�N�g��
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

		stickMoveVec.x = -stick.normalize().x;
		stickMoveVec.z = -stick.normalize().z;

		frontVec_ = cameForward * stickMoveVec.z + cameRight * stickMoveVec.x;
	}

	if (frontVec_ != 0.0f)
	{
		moveVel_ = frontVec_.normalize() * moveSpeed_;

		worldTransform_.translation += moveVel_;
		worldTransform_.rotation.y = atan2f(frontVec_.x, frontVec_.z);
	}

	// R�X�e�B�b�N�ŃJ������]
	if (JoypadInput::GetStick(PadCode::RightStick).x > deadZone ||
		JoypadInput::GetStick(PadCode::RightStick).x < -deadZone) {
		rot.y = JoypadInput::GetStick(PadCode::RightStick).x / 1000 * 2;

		joyStickInfoR.y = JoypadInput::GetStick(PadCode::RightStick).x / 1000;
	}

	// �J�����̉�]�p�̂��̂ɂ���ɃR�s�[
	cameraWorld_.translation = worldTransform_.translation;

	cameraWorld_.rotation.y += MathUtil::DegreeToRadian(rot.y);
	cameraWorld_.UpdateMatrix();
	//worldTransform_.rotation.y += MathUtil::DegreeToRadian(rot.y);
	//worldTransform_.translation += move;
	

	ImGui::Begin("joyPadInfo");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));

	ImGui::InputFloat2("joySrickL", &joyStickInfoL.x, "%.2f");
	ImGui::InputFloat2("joySrickR", &joyStickInfoR.x, "%.2f");
	ImGui::InputFloat3("playerPos", &worldTransform_.translation.x, "%.2f");

	ImGui::End();
	

	// ���[���h�s��X�V
	UpdateWorldMatrix();
}
