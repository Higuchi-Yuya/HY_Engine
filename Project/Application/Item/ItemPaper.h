#pragma once
#include "BillboardTex.h"
#include "Sprite.h"
#include "Player.h"

class ItemPaper
{
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
	/// 描画処理
	/// </summary>
	void Draw();

public:// セッター
	static void SetPlayer(Player* player) { sPlayer_ = player; }

private:// 静的メンバ変数
	static Player* sPlayer_;

public:// メンバ変数

	// フィールド上にあるときに使うキラキラのテクスチャ
	BillboardTex billTex_;

	// アイテムを拾った後に画面上に表示されるスプライト
	Sprite itemSprite_;

};

