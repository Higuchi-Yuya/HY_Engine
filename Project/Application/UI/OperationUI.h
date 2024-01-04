#pragma once
#include "Sprite.h"


class OperationUI
{
private:

	// スプライトのタイプ
	enum Type
	{
		AButtonUp,
		AButtonDown,

		LstickUnder,
		LstickCover,

		AttackText,
		MoveText,

		itemFlame1,
		itemFlame2,
		itemFlame3,

		Size,
	};
private:
	// 操作スプライト
	std::vector<std::unique_ptr<Sprite>> sprites_;

	// Aボタンのテクスチャ
	std::unique_ptr<Texture> AbuttonTex_;

	// Lスティックのテクスチャ
	std::unique_ptr<Texture> LstickTex_;

	// 移動のテキストテクスチャ
	std::unique_ptr<Texture> moveTextTex_;

	// 攻撃のテキストテクスチャ
	std::unique_ptr<Texture> attackTextTex_;

	// アイテムのフレームのテキストテクスチャ
	std::unique_ptr<Texture> itemFlameTex_;

	// Lスティックのデフォルトポジション
	Vector2 LstickDefuPos_;

	// アイテムフレームの座標
	Vector2 itemFlameDefaPos_;

	// アイテムフレームのデフォルトサイズ
	Vector2 itemFlameDefaSize_;

public:
	OperationUI();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// スプライトの描画
	/// </summary>
	void DrawFrontSprite();

};

