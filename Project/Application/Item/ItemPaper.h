#pragma once
#include "BillboardTex.h"
#include "Sprite.h"
#include "Player.h"
#include "Easing.h"

class ItemPaper
{
private:
	enum ColState
	{
		withInRange,  //範囲内
		outOfRange,	  // 範囲外
	};

	enum ItemState
	{
		sparkling,// キラキラの状態
		afterPickItUp,// 拾った後の状態
	};

public:// メンバ関数

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(std::string fileName);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 3D描画処理
	/// </summary>
	void Draw3D();

	/// <summary>
	/// 2D描画処理
	/// </summary>
	void Draw2D();

public:// セッター
	static void SetPlayer(Player* player) { sPlayer_ = player; }

public:// ゲッター
	const bool GetIsGetItem() { return IsGetItem_; }

private:// プライベート関数

	// プレイヤーとの当たり判定関連
	void OncolToPlayerUpdate();

	// 当たり範囲内と外の更新処理
	void ColStateUpdate();

	// 範囲内の場合のアイテムの更新処理
	void ItemStateUpdate();

private:// 静的メンバ変数
	static Player* sPlayer_;

public:// メンバ変数

	// フィールド上にあるときに使うキラキラのテクスチャ
	BillboardTex billTex_;

	// アイテムを拾った後に画面上に表示されるスプライト
	Sprite itemSprite_;
	std::unique_ptr<Texture> spriteTex_;

	// イージング
	Easing easeAlpha_;
	Easing easeTrans_;

	// 状態
	ColState colState_;
	ItemState itemState_;

	// アルファの加える値
	float alphaChangeValue_;

	// スケールを拡縮するフラグ
	bool IsBig_;

	// スケールの拡大と縮小の情報
	Vector3 scaleBig_;
	Vector3 scaleSmall_;

	// 拾った後アイテムを閉じるフラグ
	bool IsCheckSprite_;

	// 拾った後アイテムが表示している間に更新処理を止めるフラグ
	bool IsGetItem_;

};

