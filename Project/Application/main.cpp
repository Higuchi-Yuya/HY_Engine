#pragma region 読み込むヘッダー


//#pragma comment(lib,"d3dcompiler.lib")

#include<iostream>

#include <math.h>

#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "SpriteManager.h"
#include "LightGroup.h"
#include "GameScene.h"
#include "PostEffect.h"
#include "InputManager.h"
#include <dxgidebug.h>
#include "ParticleManager.h"
#include "PostRenderBase.h"
#include "PostEffectManager.h"

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

struct D3DResouceLeakChecker
{
	~D3DResouceLeakChecker()
	{
		// リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1>debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR,  _In_ int) {

	D3DResouceLeakChecker leckCheck;

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

	// ImGuiの初期化
	ImGuiManager* imguiManager = new ImGuiManager();
	imguiManager->Initialize(winApp,dxCommon);

	// 入力の初期化
	Input::StaticInitialize(winApp);

	InputManager* inputManager = InputManager::GetInstance().get();
	inputManager->SetWinApp(winApp);
	inputManager->Init();

	// スプライトの初期化
	std::unique_ptr<SpriteManager> spriteManager = nullptr;
	// スプライト共通部の初期化
	spriteManager = std::make_unique<SpriteManager>();
	spriteManager->Initialize(dxCommon);
	
	Sprite::StaticInitialize(spriteManager.get());

	// テクスチャの初期化
	TextureManager::StaticInitialize(dxCommon);
	
	// オブジェクトの初期化
	Object3d::StaticInitialize(dxCommon->GetDevice());

	// ビュープロジェクションの初期化
	ViewProjection::StaticInitialize(dxCommon->GetDevice());

	// サウンドの静的の初期化
	Sound::StaticInitialize();

	// ライトの静的初期化
	LightGroup::StaticInititalize(dxCommon->GetDevice());

	// フォグの静的初期化
	Fog::StaticInitialize(dxCommon->GetDevice());

	// FBXの初期化
	FbxModel::SetDevice(dxCommon->GetDevice());
	FbxModel::StaticInitialize();

	// ポストエフェクトの初期化
	PostEffect::SetDevice(dxCommon->GetDevice());

	// パーティクルの初期化
	ParticleManager::StaticInitialize(dxCommon->GetDevice());

	// ポストエフェクトレンダーベースの初期化
	PostRenderBase::GetInstance()->SetDevice(dxCommon->GetDevice());
	PostRenderBase::GetInstance()->DescriptorHeapInit();
	
	// ポストエフェクトマネージャーの初期化
	std::unique_ptr<PostEffectManager> postEffectManager = nullptr;
	postEffectManager = std::make_unique<PostEffectManager>();
	postEffectManager->SetDxCommon(dxCommon);
	postEffectManager->Initialize();

	/////////////////////////////////////////////////////////
	//--------------DirectX12初期化処理　ここまで-------------//
	///////////////////////////////////////////////////////
#pragma endregion

#pragma region シーンに使う変数の初期化


	GameScene* gameScene = new GameScene();
	gameScene->Initialize();
	gameScene->SetDxComon(dxCommon);

	postEffectManager->SetGameScene(gameScene);

	//PostEffect* postEffect = nullptr;
	//// かりに持たせるやつ
	//PostColorInversion* post = nullptr;
	//Texture tex = TextureManager::Load2DTexture("risu.jpg");

	//postEffect = new PostEffect();
	//postEffect->Initialize();

	//post = new PostColorInversion();
	//post->Initialize();



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

		InputManager::GetInstance()->Update();
		


		//////////////////////////////////////////////
		//-------DireceX毎フレーム処理　ここまで--------//
		////////////////////////////////////////////
#pragma region IMGUIの更新処理
	// ImGuiの更新処理
		imguiManager->Begin();
		gameScene->Update();
		gameScene->ImguiUpdate();
		
		imguiManager->End();
#pragma endregion

#pragma endregion

		//postEffect->PreDrawScene(dxCommon->GetCommandList());

		//postEffect->PostDrawScene(dxCommon->GetCommandList());

		//post->PreDrawScene(dxCommon->GetCommandList());
		
		//postEffect->Draw(dxCommon->GetCommandList());

		//post->PostDrawScene(dxCommon->GetCommandList());

		postEffectManager->BloomDrawSetting();

#pragma region 描画処理



		//描画コマンドここから
		dxCommon->PreDraw();
		
#pragma region 背景スプライト描画
		// 背景スプライト描画

		spriteManager->PreDraw();
		//-----ここから 背景スプライト描画 -----//

		gameScene->Draw2DBack();

		//post->Draw(dxCommon->GetCommandList());

		//-----ここまで 背景スプライト描画 -----//
		spriteManager->PostDraw();
		// 深度バッファクリア
		dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region ３Ｄモデル描画


		postEffectManager->EffectBloomDraw();
#pragma endregion
		
#pragma region 前景スプライト描画
		// 描画前処理
		
		spriteManager->PreDraw();
		//-----ここから 2D描画 -------//
		gameScene->Draw2DFront();
		

		//-----ここまで 2D描画 -------//
		// 描画後処理
		spriteManager->PostDraw();
#pragma endregion

#pragma region IMGUIの描画
		imguiManager->Draw();
#pragma endregion


		// 描画コマンドの終了
		dxCommon->PostDraw();

#pragma endregion
	}
#pragma region  WindowsAPI後始末

	//もうクラスは使わないので登録を解除する

	// ゲームシーンの解放
	delete gameScene;
	// ImGuiのマネージャーを解放
	imguiManager->Finalize();
	delete imguiManager;

	ParticleManager::StaticFinalize();
	TextureManager::StaticFinalize();
	Object3d::StaticFinalize();
	FbxModel::StaticFainalize();
	PostRenderBase::Destroy();

	// WindouwsAPIの終了処理
	winApp->Finalize();
	// WindouwsAPI解放
	delete winApp;
	// DirectXcommonの解放
	delete dxCommon;
#pragma endregion

	return 0;
}
