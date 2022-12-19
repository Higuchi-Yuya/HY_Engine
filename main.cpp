#pragma region 読み込むヘッダー

#pragma comment(lib,"d3dcompiler.lib")

#include<iostream>

#include <math.h>

#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "Object3d.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Quaternion.h"
#include "Light.h"

#pragma endregion

#pragma region おまじない
// @brief コンソール画面にフォーマット付き文字列の表示
// @param format フォーマット(%dとか%fとかの)
// @param 可変長引数
// @remarks この関数はデバック用です。デバッグ時にしか動作しません
void DebugOutputFormatString(const char* format, ...) {
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif
}


#pragma endregion

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR,  _In_ int) {

#pragma region WindowsAPI初期化処理
	// ポインタ
	WinApp* winApp = nullptr;

	// WindouwsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();

#pragma endregion

#pragma region DirectX初期化処理

	////////////////////////////////////////////////////
	//-------------DirectX12初期化処理ここから-----------//
	//////////////////////////////////////////////////

	// Commonポインタ
	DirectXCommon* dxCommon = nullptr;

	// DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initalize(winApp);

	// ポインタ
	Input* input = nullptr;

	// 入力の初期化
	input = new Input();
	input->Initialize(winApp);

	// スプライトの初期化
	SpriteManager* spriteManager = nullptr;
	// スプライト共通部の初期化
	spriteManager = new SpriteManager;
	spriteManager->Initialize(dxCommon);
	
	// テクスチャの初期化
	Texture::StaticInitialize(dxCommon);
	
	// オブジェクトの初期化
	Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height);

	// ビュープロジェクションの初期化
	ViewProjection::StaticInitialize(dxCommon->GetDevice());

	// ライトの静的初期化
	Light::StaticInititalize(dxCommon->GetDevice());

	// オブジェクト共通のライトの初期化
	Light* light = nullptr;

	// ライトの生成
	light = Light::Create();
	// ライトの色を設定
	light->SetLightDir({ 1,0,0 });
	// 3Dオブジェクトにライトをセット
	Object3d::SetLight(light);

	/////////////////////////////////////////////////////////
	//--------------DirectX12初期化処理　ここまで-------------//
	///////////////////////////////////////////////////////
#pragma endregion

#pragma region シーンに使う変数の初期化
	// スプライト一枚の初期化
	int textureHandle;
	int textureHandle2;
	textureHandle = Texture::LoadTexture("skydome/Nebura.jpg");
	textureHandle2 = Texture::LoadTexture("texture.png");
	Sprite* sprite = new Sprite();
	Sprite* sprite2 = new Sprite();
	sprite->StaticInitialize(spriteManager);
	sprite->Initialize(textureHandle, { WinApp::window_width / 2,WinApp::window_height / 2 }, { 1280,720 });
	sprite2->Initialize(textureHandle2, { 200,200 });

	Model* model = Model::LoadFromOBJ("skydome",true);
	Model* model_2 = Model::LoadFromOBJ("Medama",true);

	//Object3d* object3d = Object3d::Create();
	Object3d* obj_2 = Object3d::Create();
	
	//object3d->SetModel(model);
	//object3d->worldTransform_.position_ = { 10,0,0, };
	//object3d->worldTransform_.scale_ = { 5.0f,5.0f,5.0f };
	obj_2->SetModel(model_2);
	obj_2->worldTransform_.scale_ = { 5.0f,5.0f,5.0f };
	
	ViewProjection* view = new ViewProjection;
	view->DebugCameraInitialze(input);
	

#pragma endregion

	//ゲームループ
	while (true) {
#pragma region ウィンドウメッセージ処理

		// Windowsのメッセージ処理
		if (winApp->ProcessMessage()) {
			// ゲームループを抜ける
			break;
		}

#pragma endregion

#pragma region DirectX毎フレーム処理
		/////////////////////////////////////////////////////
		//----------DireceX毎フレーム処理　ここから------------//
		///////////////////////////////////////////////////

		// 入力の更新
		input->Update();
		
		// 数字の0キーが押されていたら
		if (input->PushKey(DIK_0))
		{
			OutputDebugStringA("Hit 0\n");  // 出力ウィンドウに「Hit 0」と表示
		}
		
		light->Update();
		//object3d->SetScale(scale_);
		//object3d->Update();
		obj_2->worldTransform_.rotation_.y += 0.01f;
		obj_2->Update();
		view->DebugCameraUpdate();

		//////////////////////////////////////////////
		//-------DireceX毎フレーム処理　ここまで--------//
		////////////////////////////////////////////

#pragma endregion

#pragma region 描画処理

		//描画コマンドここから
		dxCommon->PreDraw();
		
#pragma region 背景スプライト描画
		// 背景スプライト描画
		spriteManager->PreDraw();
		//-----ここから 背景スプライト描画 -----//
		sprite->Draw();



		//-----ここまで 背景スプライト描画 -----//
		spriteManager->PostDraw();
		// 深度バッファクリア
		dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region ３Ｄモデル描画
		Object3d::PreDraw(dxCommon->GetCommandList());
		//-----ここから 3Dモデルの描画 -----//
		
		//object3d->Draw(view);
		obj_2->Draw(view);
		
		//-----ここまで 3Dモデルの描画 -----//
		Object3d::PostDraw();
#pragma endregion
		
#pragma region 前景スプライト描画
		// 描画前処理
		spriteManager->PreDraw();
		//-----ここから 2D描画 -------//
		//sprite->Draw();
		sprite2->Draw();



		//-----ここまで 2D描画 -------//
		// 描画後処理
		spriteManager->PostDraw();
#pragma endregion

		// 描画コマンドの終了
		dxCommon->PostDraw();



#pragma endregion
	}
#pragma region  WindowsAPI後始末

	//もうクラスは使わないので登録を解除する
	// スプライトの解放
	delete sprite;
	delete sprite2;
	// オブジェクトの解放
	//delete object3d;
	delete obj_2;
	// モデルの解放
	delete model;
	delete model_2;
	// ビューの解放
	delete view;
	// ライトの解放
	delete light;

	// スプライトマネージャーの解放
	delete spriteManager;
	// 入力解放
	delete input;
	// WindouwsAPIの終了処理
	winApp->Finalize();
	// WindouwsAPI解放
	delete winApp;
	// DirectXcommonの解放
	delete dxCommon;
#pragma endregion

	return 0;
}
