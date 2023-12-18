#pragma once
#include "GaussianBlur.h"
#include "PostEffectComposition.h"
#include "PostEffectRipples.h"
#include "PostEffectScaling.h"
#include "Easing.h"
#include "Enemy.h"
#include "Player.h"

class BeatEffect
{
private://プライベートサブクラス
	enum beatState
	{
		_Expansion,// 膨張
		_Reduction,// 縮小
	};

	enum notBeatState
	{
		_SetValue,// 値の設定
		_Undo,// 元に戻す
	};

public:// メンバ関数

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 鼓動するときの処理
	/// </summary>
	void BeatUpdate();

	/// <summary>
	/// 鼓動範囲ではない時の処理
	/// </summary>
	void NotBeatUpdate();

	/// <summary>
	/// Imguiの更新処理
	/// </summary>
	void ImguiUpdate();

	/// <summary>
	/// 描画前設定
	/// </summary>
	void DrawPass();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// リセット関数
	/// </summary>
	void Reset();

public:// セッター

	/// <summary>
	/// ガウシアンブラーを設定
	/// </summary>
	/// <param name="gBlur">ガウシアンブラー</param>
	static void SetGaussianBlur(GaussianBlur* gBlur) { gaussianBlur_ = gBlur; };

	/// <summary>
	/// 敵の情報を設定
	/// </summary>
	/// <param name="enemysInfo">敵の情報のベクター配列</param>
	static void SetEnemys(std::vector<Enemy*>enemysInfo) { enemysInfo_ = enemysInfo; }

	/// <summary>
	/// プレイヤーの情報を設定
	/// </summary>
	/// <param name="player">プレイヤーのポインタ</param>
	static void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// 鼓動するかどうかを設定
	/// </summary>
	/// <param name="isBeat">ビートフラグ</param>
	void SetIsBeat(bool isBeat) { IsBeat_ = isBeat; }

	/// <summary>
	/// コマンドリストを設定
	/// </summary>
	/// <param name="cmdList">コマンドリスト</param>
	static void SetCmdList(ID3D12GraphicsCommandList* cmdList) { cmdList_ = cmdList; }

private:// 借りてくるもの

	// ガウシアンブラー(借りてくるもの)
	static GaussianBlur* gaussianBlur_;

	// エネミーの情報配列
	static std::vector<Enemy*>enemysInfo_;

	// プレイヤーの情報
	static Player* player_;

	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList_;

private:// メンバー変数

	// 合成用のポストエフェクト
	std::unique_ptr<PostEffectComposition>postComposition_;
	// 波紋のポストエフェクト
	std::unique_ptr<PostEffectRipples>postRipples_;
	// 拡大縮小用のポストエフェクト
	std::unique_ptr<PostEffectScaling>postScaling_;

	// イージング
	Easing ease_;

	// ビートするかフラグ
	bool IsBeat_;
	bool IsNotBeatDo_;

	// ビートする距離
	float beatDistance_;

	// ビートする間隔
	float beatInterval_;

	// ビートのスタート
	Vector2 beatExpansionStart_;
	Vector2 beatReductionStart_;
	Vector2 notBeatStart_;

	// ビートのエンド
	Vector2 beatExpansionEnd_;
	Vector2 beatReductionEnd_;
	Vector2 notBeatEnd_;

	// ビートしてる状態
	beatState beatState_;
	notBeatState notBeatState_;

	// 波紋タイム
	float timer_ = 0;
	float timeLimit_ = 120;
};

