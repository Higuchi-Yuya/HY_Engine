#pragma once
#include "Sprite.h"
#include "WorldTransform.h"
class TimerUI
{
private:
	enum Unit
	{
		One,
		Ten,
		Hundred,

		Size,
	};

private:
	// タイマー用のスプライト
	std::array<std::unique_ptr<Sprite>, Unit::Size> mTimerUnit;

	// 親ワールド情報
	WorldTransform mParent;

	// スプライトのサイズや間隔
	float mTexSize = 128;
	float mTexSpace = mTexSize - 16;

	// スプライトの桁
	uint32_t dight = 0;

	// スプライト用のナンバーテクスチャ
	std::unique_ptr<Texture> numberTex_;

	// タイマー
	int32_t gameTimer_ = 0;
	int32_t remainTime_ = 60 * 3;

private:
	void TextureSetting();

public:
	TimerUI();
	void Init();
	void Update();
	void DrawFrontSprite();

	void Reset();
};

