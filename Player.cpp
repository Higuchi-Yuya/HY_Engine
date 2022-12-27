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

	// モデルの初期化
	model = Model::LoadFromOBJ("player", true);

	// オブジェクトの初期化
	playerObj = Object3d::Create();
	playerObj->SetModel(model);

	playerObj->worldTransform_.position_.y = -5.5f;
}

void Player::Update()
{
	// 入力の更新
	input->Update();

	// プレイヤーの横移動
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
