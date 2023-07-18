#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
#include "Player.h"

class Enemy:public Object3d
{
public:

	// 初期化
	void Initialize(Model* model,Player* player);

	// 毎フレーム処理
	void Update()override;

	// 描画
	void Draw(ViewProjection* view)override;

	// 衝突時コールバック関数
	void OnCollision();

private:
	// 借りてくるプレイヤーの情報を一時的に保存
	Player* player_ = nullptr;

	// 当たったかどうか
	bool IsCollision = false;

	// フラグ
	bool isH = false;
	float timer = 0;
	float maxTime = 240;
};

