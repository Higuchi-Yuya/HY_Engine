#pragma once

#include "WinApp.h"
#include "DirectXCommon.h"
#include "SpriteManager.h"
#include "LightGroup.h"
#include "SceneManager.h"
#include "FbxModel.h"
#include "InputManager.h"
#include "ParticleManager.h"
#include "PostRenderBase.h"
#include "PostEffectManager.h"
#include "BillboardTex.h"

class MyGame
{
public:// メンバ関数

	/// <summary>
	/// 実行をする処理
	/// </summary>
	void Run();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:// プライベート関数

	/// <summary>
	/// 基底クラスの初期化処理
	/// </summary>
	void BaseInitialize();

	/// <summary>
	/// 更新処理内で扱うものの初期化処理
	/// </summary>
	void SceneInitialize();

	/// <summary>
	/// 背景スプライトの描画処理
	/// </summary>
	void Draw2DBack();

	/// <summary>
	/// 3Dオブジェなどの描画処理
	/// </summary>
	void Draw3D();

	/// <summary>
	/// 前面スプライトの描画処理
	/// </summary>
	void Draw2DFront();

private:// メンバ変数

	// winApp
	std::unique_ptr<WinApp>winApp;

	// DxCommon
	std::unique_ptr<DirectXCommon> dxCommon;

	// 入力マネージャー
	InputManager* inputManager = nullptr;

	// スプライトの初期化
	std::unique_ptr<SpriteManager> spriteManager;

	// Imguiのマネージャー
	std::unique_ptr<ImGuiManager> imguiManager;

	// ポストエフェクトのマネージャー
	std::unique_ptr<PostEffectManager> postEffectManager;

	// ゲームシーン
	std::unique_ptr<SceneManager> sceneManager;
};

