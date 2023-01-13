#pragma once
#include "Sprite.h"
#include "Object3d.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Quaternion.h"
#include "LightGroup.h"
#include "ImGuiManager.h"

class GameScene
{
public:// メンバ関数
	
	// デストラクタ
	~GameScene();

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// Imguiの更新処理
	void ImguiUpdate();

	// 描画処理
	void Draw2DBack();

	void Draw3D();

	void Draw2DFront();

private:// メンバ変数

	// 入力
	Input* input = nullptr;

	// オブジェクト共通のライトの初期化
	LightGroup* light = nullptr;
	
	// 点光源のステータス
	float pointLightPos[3] = { 0,0,0 };
	float pointLightColor[3] = { 1,1,1 };
	float pointLightAtten[3] = { 0.3f,0.1f,0.1f };

	// スポットライトのステータス
	Vector3 spotLightDir = { 0,-1,0 };
	Vector3 spotLightPos = { 0,5,0 };
	Vector3 spotLightColor = { 1,1,1 };
	Vector3 spotLightAtten = { 0.0f,0.0f,0.0f };
	Vector2 spotLightFactorAngle = { 20.0f,30.0f };

	// 丸影のステータス
	Vector3 circleShadowDir = { 0,-1,0 };
	Vector3 circleShadowAtten = { 0.5f,0.6f,0.0f };
	Vector2 circleShadowFactorAngle = { 0.0f,0.5f };

	Vector3 fighterPos = { 0,0.0f,0 };

	// テクスチャハンドル
	int textureHandle;
	int textureHandle2;

	// スプライト
	Sprite* sprite = nullptr;
	Sprite* sprite2 = nullptr;

	// モデル
	Model* model = nullptr;
	Model* model_2 = nullptr;
	Model* modelFighter = nullptr;
	Model* groundModel = nullptr;

	// オブジェクト
	Object3d* object3d = nullptr;
	Object3d* obj_2 = nullptr;
	Object3d* objFighter = nullptr;
	Object3d* groundObj = nullptr;


	// ビュープロジェクション
	ViewProjection* view = nullptr;

	Vector2 spritePos;
	char buf[256] = "";
	float f = 0.0f;
};

