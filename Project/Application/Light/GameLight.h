#pragma once
#include "LightGroup.h"
#include <vector>
#include <WorldTransform.h>
#include "Easing.h"
#include "Object3d.h"

// 前方宣言
class Player;
class ItemPaper;
class Enemy;

class GameLight
{
private:
	enum FlashState {
		None,// 何もないとき
		Shine,// 光る
		Atten,// 減衰
		BackDefu,// 元の状態に戻す
	};

	enum GuidLightState
	{
		ShiftTime,
		SetValue,
		Move,
	};

	struct GuidingLight
	{
		std::unique_ptr<Object3d> guidlightObj;

		bool IsShift = false;

		float startTaiming;
		float startTimer = 0;

		Easing easeGuid;
		Vector3 colPoint;

		Vector3 startPos;
		Vector3 endPos;
		Vector3 halfVel;
		Vector3 shiftVel;

		GuidLightState state;
	};

public:// メンバ関数

	~GameLight();

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// Imgui
	void ImguiUpdate();

	// 3Dモデルの描画
	void DrawForward3D(ViewProjection* view);

	/// <summary>
	/// 導きの光の更新処理
	/// </summary>
	/// <param name="itemPapers">アイテムのリスト</param>
	void GuidLightUpdate(std::vector<ItemPaper*>itemPapers, Enemy& enemy);

	/// <summary>
	/// 敵が死亡時の導きの光の処理
	/// </summary>
	/// <param name="itemPapers">アイテムのリスト</param>
	void EnemyGuidLightUpdate(std::vector<ItemPaper*>itemPapers);

private:// プライベート関数

	/// <summary>
	/// モーション終了後のライトを消す処理
	/// </summary>
	void GuidLightOffUpdate(Enemy& enemy);

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

	/// <summary>
	/// 敵の情報を設定
	/// </summary>
	/// <param name="enemysInfo">敵の情報のベクター配列</param>
	static void SetEnemys(std::vector<Enemy*>enemysInfo) { enemysInfo_ = enemysInfo; }

private:

	// エネミーの情報配列
	static std::vector<Enemy*>enemysInfo_;

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

	// 導きの光関連

	// 導きの光の番号
	UINT guidLightStartNum_;

	// 導きの光の数
	UINT guidLightNum_;
	
	// 導きの光の構造体
	std::vector<GuidingLight*> guidLights_;

	// 導きの光の実体モデル
	std::unique_ptr<Model> guidModel_;

	Vector3 guidObjScale_;

	Vector3 guidLightIdensity_;
	Vector3 guidLightRadius_;

	Vector3 guidLightColor_[3];

	Vector3 lightOffPos_;
	Vector3 enemyDeadPos_;

	int guidLightMoveCount_;
	int guidLightMoveMaxCount_;

	// 死亡している敵の数
	int enmeyDeadNum_;

	// 敵の心臓の光関連
	Vector3 heartLightColor_;
	float heartLightIndensity_;
	float heartLightRadius_;
	float heartLightDecay_;

};

