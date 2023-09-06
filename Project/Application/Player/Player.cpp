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

	bulletModel_.reset(Model::LoadFromOBJ("sphere",true));
	cameraWorld_.Initialize();

	// �v���C���[��HP�̏�����
	playerHitPoint_ = playerHitPointMax_;

	// �v���C���[�̃e�N�X�`���n���h���̏�����
	textureHandleHpBar_.reset(TextureManager::Load2DTextureP("hpBar.png"));
	textureHandleHpInside_.reset(TextureManager::Load2DTextureP("hpInside.png"));

	// �v���C���[�̃X�v���C�g�̏�����
	playerHpBar_ = std::make_unique<Sprite>();
	playerHpBar_->Initialize(textureHandleHpBar_.get());
	playerHpBar_->SetAnchorPoint({ 0,0 });

	playerHpInside_ = std::make_unique<Sprite>();
	playerHpInside_->Initialize(textureHandleHpInside_.get());
	playerHpInside_->SetAnchorPoint({ 0,0 });

	return true;
}

void Player::Update()
{
	// �����Ă���Ԃ̍X�V����
	if (IsAlive_){

		// �ړ��̍X�V����
		MoveUpdate();

		// �U���֐�
		Attack();

		// �v���C���[��HP���O�ȉ��Ȃ�v���C���[������
		if (playerHitPoint_ <= 0) {
			IsAlive_ = false;
		}
	}
	

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

void Player::OnCollision()
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

const Vector3 Player::GetFrontPos() const
{
	return Vector3(frontW_.matWorld_.m[3][0], frontW_.matWorld_.m[3][1], frontW_.matWorld_.m[3][2]);
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

const std::list<std::unique_ptr<PlayerBullet>>& Player::GetBullets() 
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

	// �n�ʔ��肵�����l
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
	moveSpeed_ /= 2;
	if (moveSpeed_ <= moveSpeedMin_) {
		moveSpeed_ = moveSpeedMin_;
	}

}

void Player::OnColUpSpeed()
{
	moveSpeed_ += 0.005f;
	if (moveSpeed_ >= moveSpeedMax_) {
		moveSpeed_ = moveSpeedMax_;
	}
}

void Player::Draw2DFront()
{
	playerHpInside_->Draw();
	playerHpBar_->Draw();
	
}

void Player::Reset()
{
	// ���[���h�ϊ��f�[�^�̃��Z�b�g
	worldTransform_.translation = { 0,0,0 };
	worldTransform_.rotation = { 0,0,0 };

	// �v���C���[��HP�̃��Z�b�g
	playerHitPoint_ = playerHitPointMax_;

	// �v���C���[�̏�Ԃ𕜊�
	IsAlive_ = true;

	// �v���C���[��HP�X�v���C�g�̏������Z�b�g
	pHpInsidePos_ = { 20,50 };
	pHpInsideSize_ = { 500,70 };
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

		if (IsAttack_ == true) {
			moveSpeed_ = 0.08f;
			moveVel_ = frontVec_.normalize() * moveSpeed_;
			
		}
		else if (IsAttack_ == false) {
			OnColUpSpeed();
			moveVel_ = frontVec_.normalize() * moveSpeed_;
			worldTransform_.rotation.y = atan2f(frontVec_.x, frontVec_.z);
		}

		// �ړ��x�N�g�������Ɉړ�
		worldTransform_.translation += moveVel_;
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
	ImGui::InputFloat3("playerRot", &worldTransform_.rotation.x, "%.2f");

	ImGui::End();
	
	ImGui::Begin("playerSprite");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));

	ImGui::InputFloat2("hpBarPos", &pHpBarPos_.x, "%.2f");
	ImGui::InputFloat2("hpInsidePos", &pHpInsidePos_.x, "%.2f");
	ImGui::InputFloat2("hpBarSize", &pHpBarSize_.x, "%.2f");
	ImGui::InputFloat2("hpInsideSize", &pHpInsideSize_.x, "%.2f");

	ImGui::End();

	// �X�v���C�g�̃|�W�V������T�C�Y�̕ύX
	playerHpBar_->SetPosition(pHpBarPos_);
	playerHpInside_->SetPosition(pHpInsidePos_);
	playerHpBar_->SetSize(pHpBarSize_);
	playerHpInside_->SetSize(pHpInsideSize_);


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
		IsAttack_ = true;

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
	else {
		IsAttack_ = false;
	}
}

void Player::OnColHitPoint()
{
	playerHitPoint_ -= 1;
	// HP�̃Z�b�g
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

	// HP�̃|�W�V�����̂��炵
	float hpMovePos = 0;
	hpMovePos = (1 - (nowHp)) * 100;
	pHpInsidePos_.x = (20 + (hpMovePos * 0.3f));

	playerHpInside_->SetPosition(pHpInsidePos_);
	playerHpInside_->SetSize(pHpInsideSize_);
}
