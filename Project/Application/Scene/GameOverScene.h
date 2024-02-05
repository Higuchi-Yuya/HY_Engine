#pragma once
#include "Sprite.h"
#include "Object3d.h"
#include "ViewProjection.h"
#include "IScene.h"

class GameOverScene:public IScene
{
public:// メンバ関数

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;

	/// <summary>
	/// 3Dオブジェクトの描画処理
	/// </summary>
	void Draw3D()override;

	/// <summary>
	/// 前面スプライトの描画処理
	/// </summary>
	void Draw2DFront()override;

	/// <summary>
	/// リセット関数
	/// </summary>
	void Reset()override;

	void ImguiUpdate()override;
	void DrawBloomObject()override;
	void DrawHighLumiObj()override;
	bool GetIsGameClear()override;

public:// 静的メンバ関数

public:// セッター

public:// ゲッター

	/// <summary>
	/// シーン終了しているかを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsSceneFinsh()override { return IsSceneFinsh_; }

	/// <summary>
	/// タイトルかゲームへのフラグを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsTitleOrGame() { return IsTitleOrGame_; }

private:// プレイべーとメンバ関数

private:// メンバ変数

	// テクスチャハンドル
	std::unique_ptr<Texture> backTexHandle_;
	std::unique_ptr<Texture> buttonTexHandle_;
	std::unique_ptr<Texture> resultGameOverFontHandle_;

	// スプライト
	std::unique_ptr<Sprite> backSprite_ = nullptr;
	std::unique_ptr<Sprite> pressASprite = nullptr;
	std::unique_ptr<Sprite> resultGameOverFont_ = nullptr;

	// シーン終了フラグ
	bool IsSceneFinsh_;

	// タイトルかもう一度を判別するフラグ
	bool IsTitleOrGame_;
};

