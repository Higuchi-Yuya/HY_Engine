#include "Player.h"

Player::Player()
{
	// ���f���̏�����
	model = Model::LoadFromOBJ("player", true);
}

Player::~Player()
{
	delete input;
	delete playerObj;
	delete model;
	delete objCollision;
}

void Player::Initialize()
{
	// ���͂̏�����
	input = new Input();
	input->Initialize();

	// �I�u�W�F�N�g�̏�����
	playerObj = Object3d::Create();
	playerObj->SetModel(model);

	objCollision = Object3d::Create();
	objCollision->SetModel(model);

	// �|�W�V�����̏�����
	playerObj->worldTransform_.position_ = {0.0f,-5.5f ,0.0f} ;
	objCollision->worldTransform_.position_ = { 0.0f,-4.0f ,0.0f };
	objCollision->worldTransform_.scale_ = playerBasketRadius;
}

void Player::Update()
{
	// ���͂̍X�V
	input->Update();

	// �v���C���[�̉��ړ�
	if (input->PushKey(DIK_A)) {
		playerObj->worldTransform_.position_.x -= 0.25f;
		objCollision->worldTransform_.position_.x -= 0.25f;
	}
	if (input->PushKey(DIK_D)) {
		playerObj->worldTransform_.position_.x += 0.25f;
		objCollision->worldTransform_.position_.x += 0.25f;
	}
	
	// �v���C���[�̈ړ�����
	if (playerObj->worldTransform_.position_.x >= 10.0f) {
		playerObj->worldTransform_.position_.x = 10.0f;
		objCollision->worldTransform_.position_.x = 10.0f;
	}
	if (playerObj->worldTransform_.position_.x <= -10.0f) {
		playerObj->worldTransform_.position_.x = -10.0f;
		objCollision->worldTransform_.position_.x = -10.0f;
	}

	// �s��̍X�V�Ɠ]��
	playerObj->Update();

	objCollision->Update();
}

void Player::Draw(ViewProjection* view)
{
	// �`��
	playerObj->Draw(view);

	//objCollision->Draw(view);
}
