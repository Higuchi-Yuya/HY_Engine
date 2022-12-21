#pragma region 読み込むヘッダー

#pragma comment(lib,"d3dcompiler.lib")

#include<iostream>

#include <math.h>


#include "WinApp.h"
#include "DirectXCommon.h"
#include "SpriteManager.h"
#include "GameScene.h"

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

	// 入力の初期化
	Input::StaticInitialize(winApp);

	// スプライトの初期化
	SpriteManager* spriteManager = nullptr;
	// スプライト共通部の初期化
	spriteManager = new SpriteManager;
	spriteManager->Initialize(dxCommon);

	// テクスチャの初期化
	Texture::StaticInitialize(dxCommon);
	
	// スプライトの初期化
	Sprite::StaticInitialize(spriteManager);

	// オブジェクトの初期化
	Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height);

	// ビュープロジェクションの初期化
	ViewProjection::StaticInitialize(dxCommon->GetDevice());

	// ライトの静的初期化
	Light::StaticInititalize(dxCommon->GetDevice());

	/////////////////////////////////////////////////////////
	//--------------DirectX12初期化処理　ここまで-------------//
	///////////////////////////////////////////////////////
#pragma endregion

#pragma region シーンに使う変数の初期化
	GameScene* gameScene = new GameScene();
	gameScene->Initialize();
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
		gameScene->Update();

		
		

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
		gameScene->Draw2Dback();



		//-----ここまで 背景スプライト描画 -----//
		spriteManager->PostDraw();
		// 深度バッファクリア
		dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region ３Ｄモデル描画
		Object3d::PreDraw(dxCommon->GetCommandList());
		//-----ここから 3Dモデルの描画 -----//
		gameScene->Draw3D();

		
		//-----ここまで 3Dモデルの描画 -----//
		Object3d::PostDraw();
#pragma endregion
		
#pragma region 前景スプライト描画
		// 描画前処理
		spriteManager->PreDraw();
		//-----ここから 2D描画 -------//
		gameScene->Draw2Dfront();



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
	delete gameScene;

	// スプライトマネージャーの解放
	delete spriteManager;
	// WindouwsAPIの終了処理
	winApp->Finalize();
	// WindouwsAPI解放
	delete winApp;
	// DirectXcommonの解放
	delete dxCommon;
#pragma endregion

	return 0;
}
