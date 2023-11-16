#pragma once
#include "PostEffectHighLumi.h"
#include "GaussianBlur.h"
#include "PostEffectComposition.h"
#include "HiguLumiTarget.h"
#include "PostTarget.h"
#include "Vignette.h"
#include "GameScene.h"


class PostEffectManager
{
private:// メンバー変数

	// 高輝度抽出のターゲット
	std::unique_ptr<HiguLumiTarget> higuLumiTarget_;

	// 高輝度抽出
	std::unique_ptr<PostEffectHighLumi> postLumi_;

	// ガウシアンブラー
	std::unique_ptr<GaussianBlur>gaussianBlur_;

	// 合成用のポストエフェクト
	std::unique_ptr<PostEffectComposition>postComposition_;

	// ターゲットシーン
	std::unique_ptr<PostTarget>postTarget_;

	// ビネット
	std::unique_ptr<Vignette>vignette_;

private:// 借りてくるもの

	// ゲームシーン
	GameScene* gameScene_ = nullptr;

	// コモン
	DirectXCommon* dxCommon_ = nullptr;

public:// メンバー関数

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// IMGUI更新処理
	void ImguiUpdate();

	// ブルーム描画処理
	void EffectBloomDraw();

	// ブルームの描画処理の設定
	void BloomDrawSetting();

public:// セッター

	// ゲームシーンのセッター
	void SetGameScene(GameScene* gameScene);

	// コモンのセッター
	void SetDxCommon(DirectXCommon* dxCommon);
};

