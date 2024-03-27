#pragma once
#include "Object3d.h"
#include "Easing.h"


class EnemyHeart
{
private:
	// ハートの状態
	enum HeartState
	{
		Beat,// 鼓動

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
	void Update(WorldTransform& worldData);

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="view">ゲームカメラ</param>
	void Draw(ViewProjection* view);

private:// プライベート関数

	/// <summary>
	/// 鼓動状態の更新処理
	/// </summary>
	void BeatUpdate();

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

};

