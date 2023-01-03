#include "Fruit.h"
#include "Player.h"

Fruit::Fruit()
{
}

Fruit::~Fruit()
{
	delete model;
	delete fruitObj;
}


void Fruit::Initialize(Vector3 Pos, Model* model)
{
	// NULLポインタチェック
	assert(model);

	this->model = model;
	
	// オブジェクトの初期化
	fruitObj = Object3d::Create();

	// モデルのセット
	fruitObj->SetModel(model);

	// 初期のポジションセット
	fruitObj->worldTransform_.position_ = Pos;

	isDeath = false;
}

void Fruit::Update(Player* player)
{
	if (isDeath == false) {
		// 落下
		fruitObj->worldTransform_.position_.y -= 0.2f;

		// プレイヤーとの当たり判定
		if (collision.boxCollision(player->GetPlayerPos(), fruitObj->worldTransform_.position_,
			player->GetBasketRadius(), fruitRadius)) {
			// 当たっていたらフルーツを消す
			isDeath = true;
			isHit = true;
		}

		// 画面外処理
		if (fruitObj->worldTransform_.position_.y <= -9.0f) {
			// 画面の外にいったら消す
			isDeath = true;
		}

		// 行列の更新と転送
		fruitObj->Update();
	}
}

void Fruit::Draw(ViewProjection* view)
{
	if (isDeath == false) {
		fruitObj->Draw(view);
	}
	
}
