#include "GameCamera.h"

void GameCamera::Initialize(const WorldTransform* worldTransform)
{
	viewProjection_.Initialize();

	
	worldTransform_.Initialize();
	
	worldTransform_.parent_ = worldTransform;
	worldTransform_.translation = { 0,5,-20 };
	worldTransform_.UpdateMatrix();
}

void GameCamera::Update()
{
	// 行列を初期化
	worldTransform_.matWorld_.identity();

	// 行列を計算
	worldTransform_.matWorld_.rotation(worldTransform_.rotation);
	worldTransform_.matWorld_.translate(worldTransform_.translation);

	// 親の行列を掛ける
	worldTransform_.matWorld_ *= worldTransform_.parent_->matWorld_;

	// ワールド行列の場所をビューに収納
	viewProjection_.eye = Vector3(worldTransform_.matWorld_.m[3][0], worldTransform_.matWorld_.m[3][1], worldTransform_.matWorld_.m[3][2]);
	
	// プレイヤーの座標を注視点にセット
	viewProjection_.target = Vector3(playerPos_.x, playerPos_.y + 3, playerPos_.z);


	//ビュープロジェクションを更新
	viewProjection_.UpdateMatrix();

	ImGui::Begin("gameCamera");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));

	//ImGui::InputFloat2("joySrick", &joyStickInfo.x, "%.2f");
	ImGui::InputFloat3("cameraEye", &viewProjection_.eye.x, "%.2f");
	ImGui::InputFloat3("cameraTraget", &viewProjection_.target.x, "%.2f");

	ImGui::End();
}
