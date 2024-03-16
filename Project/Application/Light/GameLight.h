#pragma once
#include "LightGroup.h"
#include <vector>
#include <WorldTransform.h>
#include "Easing.h"

class Player;

class GameLight
{
private:
	enum FlashState {
		None,// 何もないとき
		Shine,// 光る
		Atten,// 減衰
		BackDefu,// 元の状態に戻す
	};

public:// メンバ関数

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

public:// セッター

	/// <summary>
	/// ライトグループを設定
	/// </summary>
	/// <param name="lightGroup">ライトグループ</param>
	void SetLightGroup(LightGroup* lightGroup);

	/// <summary>
	/// ポイントライトの情報を設定
	/// </summary>
	/// <param name="pointLightsInfo">ポイントライトの情報が格納されている配列</param>
	void SetPointInfo(std::vector<WorldTransform*> pointLightsInfo);

	/// <summary>
	/// プレイヤーを取得する
	/// </summary>
	/// <param name="player">プレイヤー</param>
	void SetPlayer(Player* player);

private:
	// 借りてくるライトグループ
	LightGroup* lightGroup_;

	// ポイントライト情報の配列
	std::vector<WorldTransform*> pointLightsInfo_;

	// プレイヤーのポインタ
	Player* player_ = nullptr;

	// スポットライトのY座標
	float spotDownPosY;

	// 丸影の座標
	Vector3 circleShadowCasterPos;

	// プレイヤーのポイントライトの輝度
	float playerPointLightIndensity_;
	// プレイヤーのポイントライトの範囲
	float playerPointLightRadius_;
	// プレイヤーのポイントライトの減衰値
	float playerPointLightDecay_;

#pragma region フラッシュ関連
	// フラッシュの状態
	FlashState flashState_ = FlashState::Shine;

	// フラッシュのカウント
	int flashCount_;
	int flashCountMax_;

	// フラッシュの状態ごとの輝度の値
	float flashDefuValue_;
	float flashMiddleValue_;
	float flashMaxValue_;
	float flashNowValue_;

	// フラッシュ用のイージング
	Easing easeFlash_;
	int easeFlashMiddleTimeLimit_;
	int easeFlashMaxTimeLimit_;

#pragma endregion
};

