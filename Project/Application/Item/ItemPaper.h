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
	/// 行列の更新など
	/// </summary>
	void UpdateWorldMatrix();

	/// <summary>
	/// 3D描画処理
	/// </summary>
	void Draw3D();

	/// <summary>
	/// 2D描画処理
	/// </summary>
	void Draw2D();

	/// <summary>
	/// アイテムのリセット
	/// </summary>
	void Reset();

public:// セッター

	/// <summary>
	/// 共通のプレイヤー情報を設定
	/// </summary>
	/// <param name="player">プレイヤーのポインタ</param>
	static void SetPlayer(Player* player) { sPlayer_ = player; }

	/// <summary>
	/// ポジションの設定
	/// </summary>
	/// <param name="wTranslation">ポジション</param>
	void SetWorldTransformPos(Vector3 wTranslation);

	/// <summary>
	/// キーアイテムなのかを設定
	/// </summary>
	/// <param name="isKeyItem">キーアイテムフラグ</param>
	void SetIsKeyItem(bool isKeyItem);

public:// ゲッター

	/// <summary>
	/// アイテムを獲得しているかを取得
	/// </summary>
	/// <returns></returns>
	const bool GetIsGetItem() { return IsGetItem_; }

	/// <summary>
	/// キーアイテムなのかを取得
	/// </summary>
	/// <returns></returns>
	const bool GetIsKeyItem() { return IsKeyItem_; }

	/// <summary>
	/// アイテムの表示を確認終えているかを取得
	/// </summary>
	/// <returns></returns>
	const bool GetIsCheckItem() { return IsCheckSprite_; }

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
private:
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

	// キーアイテムなのかどうか
	bool IsKeyItem_;
};

