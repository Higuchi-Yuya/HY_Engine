#pragma once
#include "Object3d.h"
class Player:public Object3d
{
public:
	// 3Dオブジェクト生成
	static Player* Create(Model* model = nullptr);

public:
	~Player();
	// 初期化
	bool Initialize()override;

	// 毎フレーム処理
	void Update()override;

	// 描画
	void Draw(ViewProjection* view)override;

	// 衝突時コールバック関数
	void OnCollision(const CollisionInfo& info)override;

private:
	Model* atariModel = nullptr;
	Object3d* atari = nullptr;

	Input* input = new Input;
};

