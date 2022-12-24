#pragma once
#include "Input.h"
#include "Sprite.h"
#include "Object3d.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Quaternion.h"
#include "Light.h"
#include "Player.h"

class GameScene
{
public:
	// コンストラクタ
	GameScene();
	// デストラクタ
	~GameScene();

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw2Dback();

	void Draw3D();

	void Draw2Dfront();

private:// メンバ変数

	// インプット
	Input* input = nullptr;

	// オブジェクト共通のライト
	Light* light = nullptr;

	// テクスチャハンドル
	int textureHandle = 0;
	int textureHandle2 = 0;

	// スプライト
	Sprite* sprite = nullptr;
	Sprite* sprite2 = nullptr;

	// モデル
	
	Model* model_2 = nullptr;

	// オブジェクト
	Player* player = nullptr;
	Object3d* obj_2 = nullptr;

	// ビュープロジェクション
	ViewProjection* view = nullptr;


};

