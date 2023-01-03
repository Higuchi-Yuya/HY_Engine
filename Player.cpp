#include "Player.h"

Player::Player()
{
	// モデルの初期化
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
	// 入力の初期化
	input = new Input();
	input->Initialize();

	// オブジェクトの初期化
	playerObj = Object3d::Create();
	playerObj->SetModel(model);

	objCollision = Object3d::Create();
	objCollision->SetModel(model);

	// ポジションの初期化
	playerObj->worldTransform_.position_ = {0.0f,-5.5f ,0.0f} ;
	objCollision->worldTransform_.position_ = { 0.0f,-4.0f ,0.0f };
	objCollision->worldTransform_.scale_ = playerBasketRadius;
}

void Player::Update()
{
	// 入力の更新
	input->Update();

	// プレイヤーの横移動
	if (input->PushKey(DIK_A)) {
		playerObj->worldTransform_.position_.x -= 0.25f;
		objCollision->worldTransform_.position_.x -= 0.25f;
	}
	if (input->PushKey(DIK_D)) {
		playerObj->worldTransform_.position_.x += 0.25f;
		objCollision->worldTransform_.position_.x += 0.25f;
	}
	
	// プレイヤーの移動制限
	if (playerObj->worldTransform_.position_.x >= 10.0f) {
		playerObj->worldTransform_.position_.x = 10.0f;
		objCollision->worldTransform_.position_.x = 10.0f;
	}
	if (playerObj->worldTransform_.position_.x <= -10.0f) {
		playerObj->worldTransform_.position_.x = -10.0f;
		objCollision->worldTransform_.position_.x = -10.0f;
	}

	// 行列の更新と転送
	playerObj->Update();

	objCollision->Update();
}

void Player::Draw(ViewProjection* view)
{
	// 描画
	playerObj->Draw(view);

	//objCollision->Draw(view);
}
