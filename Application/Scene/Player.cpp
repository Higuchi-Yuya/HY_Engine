#include "Player.h"
#include "Input.h"
#include "SphereCollider.h"

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
	delete atariModel;
	delete atari;

}

bool Player::Initialize()
{
	if (!Object3d::Initialize()) {
		return false;
	}
	input->Initialize();
	// コライダーの追加
	float radius = 0.6f;

	// 半径分だけ足元から浮いた座標を球の中心にする
	SetCollider(new SphereCollider(Vector3(0, radius, 0), radius));

	atariModel = new Model;
	atariModel = Model::LoadFromOBJ("sphere");

	atari = Object3d::Create();
	atari->SetModel(atariModel);
	//atari->worldTransform_.position_.y -= 8.0f;
	return true;
}

void Player::Update()
{
	
	input->Update();
	atari->Update();
	// A,Dで旋回
	if (input->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= 0.2f;
	}
	else if (input->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += 0.2f;
	}

	// 移動ベクトルをY軸周りの角度で回転
	Vector3 move = { 0,0,0.1f };
	Matrix4 matRot;
	matRot.rotateY(worldTransform_.rotation_.y);
	move = matRot.transformNotW(move, matRot);

	// 向いてる方向に移動
	if (input->PushKey(DIK_S)) {
		worldTransform_.position_ -= move;
	}
	else if (input->PushKey(DIK_W)) {
		worldTransform_.position_ += move;
	}

	// 行列の更新など
	Object3d::Update();
}

void Player::Draw(ViewProjection* view)
{
	Object3d::Draw(view);
	atari->Draw(view);
}

void Player::OnCollision(const CollisionInfo& info)
{
	// 衝突したらカラーを変える
	atari->worldTransform_.position_ = this->worldTransform_.position_;
	//worldTransform_.color_ = { 0.1f,1.0f,0.1f,1.0f };
	//worldTransform_.UpdateMatrix();
}
