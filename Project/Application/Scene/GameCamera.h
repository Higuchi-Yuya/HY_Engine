#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "ImGuiManager.h"
#include "Easing.h"
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
	void Initialize();

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

	// ドアの開くときのカメラ処理なのかを取得
	bool& GetIsDoorOpen() { return IsDoorOpen_; }

	// ドアの開く間のカメラ処理が終わっているのかを取得
	const bool GetIsFinishDoorOpen_() { return IsFinshDoor_; }

public:// セッター
	// 最初にセットするカメラ情報
	void SetCameraFPos(Vector3 pos);

	// カメラのポジションをセット
	void SetCameraPos(Vector3 pos);

	// タイトルのカメラの更新処理を行うかをセット
	void SetIsCanEase(bool IsCanEase);

	// ドアが開くときのカメラをセット
	void SetIsDoorOpen(bool isDoorOpen);

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

	Easing ease_;

	// イージングのタイマー
	float easeTimer_ = 0;
	float easeTimeMax_ = 180;

	bool IsCanEase_ = false;
	bool IsEaseEnd_ = false;

	// ドアが開く時のカメラ関連

	// ドアの時のカメラ座標とターゲット座標
	Vector3 doorOpenEye_ = { -0.8f,1.5f,-8.0f };
	Vector3 doorOpenTarget_ = { -0.8f,2.0f,-20.0f };

	// ドアが開くかどうか
	bool IsDoorOpen_ = false;
	bool IsFinshDoor_ = false;

	// ドア開くときのカメラの滞在時間
	float doorOpenTimer_ = 0;
	float doorOpenTimeLimit_ = 60 * 10;
};

