#pragma once
#include "Object3d.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Quaternion.h"
#include "Light.h"
#include "Input.h"
class Player
{
public:
	Player();
	~Player();

	// 初期化
	void Initialize();
	// 更新処理
	void Update();
	// 描画処理
	void Draw(ViewProjection *view);

	// ゲッター
	// ポジション
	const Vector3& GetPlayerPos() const {
		Vector3 pos = playerObj->worldTransform_.position_;
		pos.y += 1.5f;
		return  pos;}

	// かごの半径
	const Vector3& GetBasketRadius() const { return playerBasketRadius; }

private:
	// インプット
	Input* input = nullptr;

	// モデル
	Model* model = nullptr;

	// オブジェクト
	Object3d* playerObj = nullptr;

	Object3d* objCollision;

	// プレイヤーのかごの大きさ
	Vector3 playerBasketRadius = { 0.8f,0.2f,1.0f };
};

