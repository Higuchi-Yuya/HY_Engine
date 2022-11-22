#pragma region 読み込むヘッダー

#pragma comment(lib,"d3dcompiler.lib")


#include<iostream>

#include <math.h>

#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "SpriteManager.h"
#include "Sprite.h"

using namespace Microsoft::WRL;
using namespace std;


#pragma endregion

const float PI = 3.141592f;

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
	spriteManager->Initialize(dxCommon->GetDevice());
	
	/////////////////////////////////////////////////////////
	//--------------DirectX12初期化処理　ここまで-------------//
	///////////////////////////////////////////////////////
#pragma endregion

#pragma region シーンに使う変数の初期化
	// スプライト一枚の初期化
	Sprite* sprite = new Sprite();
	sprite->Initialize();
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

		

		

		//2.描画先の変更
		//レンダーターゲットビューのハンドルを取得
		
		

		

		

		////トリガー処理
		//bool キーを押した状態か(uint8_t キー番号);
		//bool キーを離した状態か(uint8_t キー番号);
		//bool キーを押した瞬間か(uint8_t キー番号);
		//bool キーを離した瞬間か(uint8_t キー番号);






		
		



		//////////////////////////////////////////////
		//-------DireceX毎フレーム処理　ここまで--------//
		////////////////////////////////////////////

#pragma endregion

#pragma region グラフィックスコマンド

		//4.描画コマンドここから
		dxCommon->PreDraw();
		// 描画前処理

		// 描画後処理
	
		//// 深度バッファクリア
		dxCommon->ClearDepthBuffer();
		//Meshの描画--------------------------------------------------------------//
		
		
		//
		//for (int i = 0; i < 20; i++)
		//{
		//	line[i].Line_Draw(indices_count2, dxCommon->GetCommandList());
		//}
		//4.描画コマンドここまで
		
#pragma region 前景スプライト描画
// 描画前処理
		

		

		// 描画後処理
	
#pragma endregion


		

#pragma region 画面入れ替え
		dxCommon->PostDraw();
#pragma endregion


#pragma endregion




	}
#pragma region  WindowsAPI後始末

	//もうクラスは使わないので登録を解除する
	delete sprite;
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

	//Render_basic::DestroyInstance();

	return 0;
}
