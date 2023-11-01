#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "ImGuiManager.h"

class GameCamera
{
private:
	// 状態
	enum TitleCameraState
	{
		FirstMove,
		SecondMove,

	};

public:// メンバ関数
	void Initialize(const WorldTransform* worldTransform);

	void TitleUpdate();

	void GameUpdate();

	void Reset();

public:// ゲッター

	// ビュープロジェクションを取得
	ViewProjection& GetView() { return viewProjection_; }

	// タイトルのカメラの処理をするかどうか
	bool& GetIsCanEase() { return IsCanEase_; }

	// タイトルのカメラ処理が終わっているかを取得
	bool& GetIsEaseEnd() { return IsEaseEnd_; }

public:// セッター
	// 最初にセットするカメラ情報
	void SetCameraFPos(Vector3 pos);

	// カメラのポジションをセット
	void SetCameraPos(Vector3 pos);

	// タイトルのカメラの更新処理を行うかをセット
	void SetIsCanEase(bool IsCanEase);

private:// プライベートメンバ関数
	void RotUpdate();

private:// メンバ変数

	//ワールド変換データ
	WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection viewProjection_;
	// 注視点のY座標を変更する値
	Vector3 offSet;


	// 回転
	float aInfo;
	Vector3 angle_;
	Vector3 cameraVecRot_;
	Vector3 dirVec;
	float velLength;

	// タイトルの最初のポジション
	Vector3 titleCameraFPos_;
	Vector3 titleCameraFTargetPos_;
	const Vector3 cameraFPos = { -15,5,-15 };
	Vector3 rotPos= { -15,15,-15 };
	Vector3 rotNorm;
	Vector3 vel;
	Vector3 rotSpeed_ = { 5,5,5 };

	// 注視点となるプレイヤーの座標
	Vector3 playerPos_;

	// 
	TitleCameraState titleCameraState_;

	// イージングのタイマー
	float easeTimer_ = 0;
	float easeTimeMax_ = 180;

	bool IsCanEase_ = false;
	bool IsEaseEnd_ = false;
};

