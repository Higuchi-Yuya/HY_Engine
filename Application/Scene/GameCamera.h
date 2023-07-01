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


private:// メンバ変数

	//ワールド変換データ
	WorldTransform worldTransform_;
	//ビュープロジェクション
	ViewProjection viewProjection_;

	Vector3 playerPos_;
};

