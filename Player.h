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
	~Player();

	// 初期化
	void Initialize();
	// 更新処理
	void Update();
	// 描画処理
	void Draw(ViewProjection *view);
private:
	// インプット
	Input* input = nullptr;

	// モデル
	Model* model = nullptr;

	// オブジェクト
	Object3d* playerObj = nullptr;

};

