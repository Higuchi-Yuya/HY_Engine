#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "ImGuiManager.h"

class GameCamera
{
public:// メンバ関数
	void Initialize(const WorldTransform* worldTransform);

	void TitleUpdate();

	void GameUpdate();

	ViewProjection& GetView() { return viewProjection_; }

	void SetCameraFPos(Vector3 pos);

	void SetCameraPos(Vector3 pos);

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
	const Vector3 cameraFPos = { -15,5,-15 };;
	Vector3 rotPos= { -15,15,-15 };
	Vector3 rotNorm;
	Vector3 vel;
	Vector3 rotSpeed_ = { 5,5,5 };

	// 注視点となるプレイヤーの座標
	Vector3 playerPos_;
};

