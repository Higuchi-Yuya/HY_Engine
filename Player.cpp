#include "Player.h"

Player::Player()
{
	// ���͂̏�����
	input = new Input();
	input->Initialize();

	// ���f���̏�����
	model = Model::LoadFromOBJ("player", true);

	// �I�u�W�F�N�g�̏�����
	playerObj = Object3d::Create();
	playerObj->SetModel(model);
}

Player::~Player()
{
	delete input;
	delete playerObj;
	delete model;

}

void Player::Initialize()
{
	// �|�W�V�����̏�����
	playerObj->worldTransform_.position_ = {0.0f,-5.5f ,0.0f} ;
	playerObj->Update();
}

void Player::Update()
{
	// ���͂̍X�V
	input->Update();

	// �v���C���[�̉��ړ�
	if (input->PushKey(DIK_A)) {
		playerObj->worldTransform_.position_.x -= 0.25f;
		
	}
	if (input->PushKey(DIK_D)) {
		playerObj->worldTransform_.position_.x += 0.25f;
		
	}
	
	// �v���C���[�̈ړ�����
	if (playerObj->worldTransform_.position_.x >= 10.0f) {
		playerObj->worldTransform_.position_.x = 10.0f;
		
	}
	if (playerObj->worldTransform_.position_.x <= -10.0f) {
		playerObj->worldTransform_.position_.x = -10.0f;
		
	}

	// �s��̍X�V�Ɠ]��
	playerObj->Update();

}

void Player::Draw(ViewProjection* view)
{
	// �`��
	playerObj->Draw(view);

}
