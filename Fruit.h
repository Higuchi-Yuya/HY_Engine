#pragma once
#include <cassert>
#include "Object3d.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Collision.h"
class Player;

// フルーツのタイプ
enum class fruitType {
	apple,
	orange,
	banana,
	bomb
};
class Fruit
{

public:
	Fruit();
	~Fruit();

	// 初期化
	void Initialize(Vector3 Pos,Model* model);
	// 更新処理
	void Update();
	// 描画処理
	void Draw(ViewProjection* view);

	void SetPlayer(Player* player) { player_ = player; }

	// 死亡フラグを渡す
	bool IsDeath() const { return isDeath; }

	// ヒットフラグを渡す
	bool IsHit()const { return isHit; }

	// 加算フラグを渡す
	bool IsPlus() const { return isPlus; }

	void SetIsPlus(bool isPlus) { this->isPlus = isPlus; }

	// 種類の番号
	int typeNum = 0;

private:
	// モデル
	Model* model = nullptr;

	// オブジェクト
	Object3d* fruitObj = nullptr;

	// 消滅フラグ
	bool isDeath = false;

	// 衝突フラグ
	bool isHit = false;

	// 点数が加算されかのフラグ
	bool isPlus = false;

	// フルーツの一個の半径
	Vector3 fruitRadius = { 1.0f,0.8f,1.0f };

	// 当たり判定
	Collision collision;

	Player* player_ = nullptr;
};

