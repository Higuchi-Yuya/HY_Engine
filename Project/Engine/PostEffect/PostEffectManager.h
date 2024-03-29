#pragma once
#include "PostEffectHighLumi.h"
#include "GaussianBlur.h"
#include "PostEffectComposition.h"
#include "HiguLumiTarget.h"
#include "PostTarget.h"
#include "SceneManager.h"
#include "BeatEffect.h"
#include "DeferredObject3d.h"
#include "DeferredRender.h"
#include "PostDefrred.h"

class PostEffectManager
{
private:// メンバー変数

	// 高輝度抽出のターゲット
	std::unique_ptr<HiguLumiTarget> higuLumiTarget_;

	// 高輝度抽出
	std::unique_ptr<PostEffectHighLumi> postLumi_;

	// ガウシアンブラー
	std::unique_ptr<GaussianBlur>gaussianBlur_;

	// ターゲットシーン
	std::unique_ptr<PostTarget>postTarget_;

	// 鼓動エフェクト
	std::unique_ptr<BeatEffect>beatEffect_;

	// テスト用のディファードオブジェクト
	std::unique_ptr<Model> boxModel_ = nullptr;
	std::unique_ptr<DeferredObject3d>testDObj;

	std::unique_ptr<DeferredRender>testDefrredRender_;

	std::unique_ptr<PostDefrred> postDeferred_;

private:// 借りてくるもの

	// ゲームシーン
	SceneManager* sceneManager_ = nullptr;

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
	void SetSceneManager(SceneManager* sceneManager);

	// コモンのセッター
	void SetDxCommon(DirectXCommon* dxCommon);

public:// ゲッター

	/// <summary>
	/// 鼓動エフェクトを取得
	/// </summary>
	/// <returns>鼓動エフェクトのポインタ</returns>
	BeatEffect* GetBeatEffcet() { return beatEffect_.get(); }
};

