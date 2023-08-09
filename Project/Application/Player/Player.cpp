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

	frontW_.Initialize();
	frontW_.translation = { 0,0,1 };
	frontW_.parent_ = &worldTransform_;

	bulletModel_.reset(Model::LoadFromOBJ("doragon"));
	cameraWorld_.Initialize();
	return true;
}

void Player::Update()
{
	// �ړ��̍X�V����
	MoveUpdate();

	// �U���֐�
	Attack();

	//�e�X�V
	for (std::unique_ptr<PlayerBullet>& bullet : bullets_) {
		bullet->Update();
	}

	// �s��̍X�V�Ȃ�
	Object3d::Update();
}

void Player::Draw(ViewProjection* view)
{
	Object3d::Draw(view);
	//�e�`��
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

const std::list<std::unique_ptr<PlayerBullet>>& Player::GetBullets() 
{
	return bullets_;
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

	// �����Ă�����Ɉړ�

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

	// �J�����̉�]�p�̂��̂ɂ���ɃR�s�[
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
	// ���[���h�s��X�V
	UpdateWorldMatrix();
}

void Player::Attack()
{
	//�f�X�t���O�̗������e���폜
	bullets_.remove_if([](std::unique_ptr<PlayerBullet>& bullet) { return bullet->IsDead(); });
	bulletInterTimer--;
	if (JoypadInput::GetButton(PadCode::ButtonA))//input_->PushKey(DIK_Y)) {
	{
		

		if (bulletInterTimer <= 0) {

			//�e�̑��x
			const float kBulletSpeed = 0.3f;
			Vector3 velocity;
			Vector3 frontVec = { frontW_.matWorld_.m[3][0],frontW_.matWorld_.m[3][1] ,frontW_.matWorld_.m[3][2] };


			// ���@����Ə��I�u�W�F�N�g�ւ̃x�N�g��
			velocity = frontVec - worldTransform_.translation;
			velocity.normalize();
			velocity *= kBulletSpeed;

			//���x�x�N�g�������@�̌����ɍ��킹�ĉ�]������
			// velocity = velocity * worldTransform_.matWorld_;

			//�e�𐶐����A������
			std::unique_ptr<PlayerBullet> newBullet = std::make_unique<PlayerBullet>();
			newBullet->Initialize(bulletModel_.get(), worldTransform_, velocity);

			//�e��o�^����
			bullets_.push_back(std::move(newBullet));

			bulletInterTimer = bulletInterval;
		}

	}
}
