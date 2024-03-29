#pragma once
#include "Object3d.h"
#include "Easing.h"


class EnemyHeart
{
private:
	// ハートの状態
	enum HeartState
	{
		Spawn,// 生成時
		Beat,// 鼓動
		BeatReduction,// 心臓の縮小
	};

	enum HeartScaleState
	{
		Expansion,// 膨張
		Reduction,// 縮小
	};

public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="worldData">敵のワールドデータ</param>
	void Update(WorldTransform& worldData,bool isLightMoveEnd);

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="view">ゲームカメラ</param>
	void Draw(ViewProjection* view);

public:// ゲッター

	/// <summary>
	/// 心臓の縮小処理が終了しているかを取得
	/// </summary>
	/// <returns></returns>
	const bool GetIsSmallEnd() { return IsSmallEnd_; }

private:// プライベート関数

	/// <summary>
	/// 生成時の更新処理
	/// </summary>
	void SpawnUpdate();

	/// <summary>
	/// 鼓動状態の更新処理
	/// </summary>
	void BeatUpdate();

	/// <summary>
	/// 心臓が縮小するときの処理
	/// </summary>
	void BeatReductionUpdate();

private:

	// ハートオブジェクト
	std::unique_ptr<Object3d> heartObj_;
	std::unique_ptr<Object3d> heartShadowObj_;

	// ハートモデル
	std::unique_ptr<Model>heartModel_;

	// イージング
	Easing easeHeart_;

	// 状態
	HeartState state_;
	HeartScaleState scaleState_;

	// 蠢きステータス
	float writhingTimer_;
	float writhingTimeMax_;

	Vector3 beatDefuScale_;
	Vector3 beatScale_;

	// 拡縮のステータス
	Vector3 defuScale_;
	Vector3 bigScale_;

	// 心臓の縮小関連
	Vector3 smallValue_;
	Vector3 minScale_;
	bool IsSmallEnd_;
	bool IsLightEnd_;

	// 生成関連
	Vector3 spawnValue_;
	Vector3 spawnScale_;
};

