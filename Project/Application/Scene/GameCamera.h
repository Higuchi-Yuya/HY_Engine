#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "ImGuiManager.h"
class GameCamera
{
public:// メンバ関数
	void Initialize(const WorldTransform* worldTransform);

	void Update();

	ViewProjection& GetView() { return viewProjection_; }

	void SetCameraPos(Vector3 pos) { playerPos_ = pos; }

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
	Vector3 angle_;
	Vector3 cameraVecRot_;
	Vector3 dir_;
	Vector3 rotSpeed_ = { 5,5,5 };

	// 注視点となるプレイヤーの座標
	Vector3 playerPos_;
};

