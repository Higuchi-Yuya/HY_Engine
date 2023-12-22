#include "MyGame.h"

void MyGame::Run()
{
	// いろんな初期化処理
	Initialize();

	//ゲームループ
	while (true) {
	#pragma region ウィンドウメッセージ処理

		// Windowsのメッセージ処理
		if (winApp->ProcessMessage()) {
			// ゲームループを抜ける
			break;
		}

#pragma endregion

		// 更新処理
		Update();

		// 描画処理
		Draw();
	}

	// 終了後の処理
	Finalize();
}

void MyGame::Initialize()
{
	// 基底クラスたちを初期化や静的なものの初期化
	BaseInitialize();

	// 更新処理内で扱うものの初期化
	SceneInitialize();
}

void MyGame::Finalize()
{
#pragma region  WindowsAPI後始末
	//もうクラスは使わないので登録を解除する

	// ImGuiのマネージャーを解放
	imguiManager->Finalize();

	// 諸々ほかの解放処理
	ParticleManager::StaticFinalize();
	TextureManager::StaticFinalize();
	Object3d::StaticFinalize();
	FbxModel::StaticFainalize();
	PostRenderBase::Destroy();

	// WindouwsAPIの終了処理
	winApp->Finalize();

#pragma endregion
}

void MyGame::Update()
{
#pragma region DirectX毎フレーム処理
	/////////////////////////////////////////////////////
	//----------DireceX毎フレーム処理　ここから------------//
	///////////////////////////////////////////////////

	InputManager::GetInstance()->Update();
	postEffectManager->Update();


	//////////////////////////////////////////////
	//-------DireceX毎フレーム処理　ここまで--------//
	////////////////////////////////////////////
	#pragma region IMGUIの更新処理
	// ImGuiの更新処理
	imguiManager->Begin();

	sceneManager->Update();
	sceneManager->ImguiUpdate();
	postEffectManager->ImguiUpdate();

	imguiManager->End();
	#pragma endregion

#pragma endregion
}

void MyGame::Draw()
{
	// ポストエフェクトの描画設定
	postEffectManager->BloomDrawSetting();

	//描画コマンドここから
	dxCommon->PreDraw();

	// 背景スプライトの描画
	Draw2DBack();

	// 3Dオブジェクトなどの描画
	Draw3D();

	// 前面スプライトの描画
	Draw2DFront();

	#pragma region IMGUIの描画
	//imguiManager->Draw();
#pragma endregion

	// 描画コマンドの終了
	dxCommon->PostDraw();

}

void MyGame::BaseInitialize()
{
#pragma region WindowsAPI初期化処理
	winApp = std::make_unique<WinApp>();
	winApp->Initialize();
#pragma endregion

#pragma region DirectX初期化処理

	////////////////////////////////////////////////////
	//-------------DirectX12初期化処理ここから-----------//
	//////////////////////////////////////////////////

	// DirectXの初期化
	dxCommon = std::make_unique<DirectXCommon>();
	dxCommon->Initalize(winApp.get());

	// ImGuiの初期化
	imguiManager = std::make_unique<ImGuiManager>();
	imguiManager->Initialize(winApp.get(), dxCommon.get());

	// 入力の初期化
	Input::StaticInitialize(winApp.get());

	inputManager = InputManager::GetInstance().get();
	inputManager->SetWinApp(winApp.get());
	inputManager->Init();

	// スプライト共通部の初期化
	spriteManager = std::make_unique<SpriteManager>();
	spriteManager->Initialize(dxCommon.get());

	Sprite::StaticInitialize(spriteManager.get());

	// テクスチャの初期化
	TextureManager::StaticInitialize(dxCommon.get());

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

	// パーティクルの初期化
	ParticleManager::StaticInitialize(dxCommon->GetDevice());

	// ポストエフェクトレンダーベースの初期化
	PostRenderBase::GetInstance()->SetDevice(dxCommon->GetDevice());
	PostRenderBase::GetInstance()->DescriptorHeapInit();

	// ポストエフェクトマネージャーの初期化
	postEffectManager = std::make_unique<PostEffectManager>();
	postEffectManager->SetDxCommon(dxCommon.get());
	postEffectManager->Initialize();

	/////////////////////////////////////////////////////////
	//--------------DirectX12初期化処理　ここまで-------------//
	///////////////////////////////////////////////////////
#pragma endregion
}

void MyGame::SceneInitialize()
{
#pragma region シーンに使う変数の初期化

	sceneManager = std::make_unique<SceneManager>();
	sceneManager->SetDxComon(dxCommon.get());
	sceneManager->Initialize();
	sceneManager->SetBeatEffect(postEffectManager->GetBeatEffcet());

	postEffectManager->SetSceneManager(sceneManager.get());

#pragma endregion
}

void MyGame::Draw2DBack()
{
#pragma region 背景スプライト描画
	// 背景スプライト描画

	spriteManager->PreDraw();
	//-----ここから 背景スプライト描画 -----//

	sceneManager->Draw2DBack();

	//-----ここまで 背景スプライト描画 -----//
	spriteManager->PostDraw();
	// 深度バッファクリア
	dxCommon->ClearDepthBuffer();
#pragma endregion
}

void MyGame::Draw3D()
{
#pragma region ３Ｄモデル描画
	postEffectManager->EffectBloomDraw();
#pragma endregion
}

void MyGame::Draw2DFront()
{
#pragma region 前景スプライト描画
	// 描画前処理

	spriteManager->PreDraw();
	//-----ここから 2D描画 -------//
	sceneManager->Draw2DFront();

	//-----ここまで 2D描画 -------//
	// 描画後処理
	spriteManager->PostDraw();

#pragma endregion
}
