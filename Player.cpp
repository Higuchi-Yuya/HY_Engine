#include "Player.h"

Player::~Player()
{
	delete input;
	delete playerObj;
	delete model;
}

void Player::Initialize()
{
	input = new Input();
	input->Initialize();

	// ���f���̏�����
	model = Model::LoadFromOBJ("player", true);

	// �I�u�W�F�N�g�̏�����
	playerObj = Object3d::Create();
	playerObj->SetModel(model);

	playerObj->worldTransform_.position_.y = -5.5f;
}

void Player::Update()
{
	// ���͂̍X�V
	input->Update();

	// �v���C���[�̉��ړ�
	if (input->PushKey(DIK_A)) {
		playerObj->worldTransform_.position_.x -= 0.2f;
	}
	if (input->PushKey(DIK_D)) {
		playerObj->worldTransform_.position_.x += 0.2f;
	}


	playerObj->Update();
}

void Player::Draw(ViewProjection* view)
{
	playerObj->Draw(view);
}
