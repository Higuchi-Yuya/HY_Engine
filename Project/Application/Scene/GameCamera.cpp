#include "GameCamera.h"
#include "JoyPadInput.h"
#include "MathUtil.h"

void GameCamera::Initialize(const WorldTransform* worldTransform)
{
	viewProjection_.Initialize();

	
	worldTransform_.Initialize();
	
	worldTransform_.parent_ = worldTransform;
	worldTransform_.translation = { -15,15,-15 };
	worldTransform_.UpdateMatrix();
}

void GameCamera::Update()
{
	// カメラ回転
	




	RotUpdate();

	// 親の行列を掛ける
	//worldTransform_.matWorld_ *= worldTransform_.parent_->matWorld_;
	
	// ワールド行列の場所をビューに収納
	viewProjection_.eye = Vector3(worldTransform_.matWorld_.m[3][0], worldTransform_.matWorld_.m[3][1], worldTransform_.matWorld_.m[3][2]);
	
	// プレイヤーの座標を注視点にセット
	viewProjection_.target = Vector3(playerPos_.x, playerPos_.y + chageYPos, playerPos_.z);


	//ビュープロジェクションを更新
	viewProjection_.UpdateMatrix();

	ImGui::Begin("gameCamera");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));

	//ImGui::InputFloat2("joySrick", &joyStickInfo.x, "%.2f");
	ImGui::InputFloat("chagePos Y", &chageYPos);
	ImGui::InputFloat3("worldRot", &worldTransform_.rotation.x, "%.2f");
	ImGui::InputFloat3("worldPos", &worldTransform_.translation.x, "%.2f");
	ImGui::InputFloat3("cameraEye", &viewProjection_.eye.x, "%.2f");
	ImGui::InputFloat3("cameraTraget", &viewProjection_.target.x, "%.2f");

	ImGui::End();
}

void GameCamera::RotUpdate()
{
	//Vector3 cameraPos = worldTransform_.translation;//Vector3(worldTransform_.matWorld_.m[3][0], worldTransform_.matWorld_.m[3][1], worldTransform_.matWorld_.m[3][2]);
	//// 方向ベクトルを求める
	//Vector3 dirVec = playerPos_ - cameraPos;
	//dirVec.normalize();
	//dir_ = { 0,0,0 };

	//// Rスティックでカメラ回転
	//if (JoypadInput::GetStick(PadCode::RightStick).x > 450 ||
	//	JoypadInput::GetStick(PadCode::RightStick).x < -450) {
	//	angle_.x = JoypadInput::GetStick(PadCode::RightStick).x / 1000 * 2;
	//	angle_.z = JoypadInput::GetStick(PadCode::RightStick).y / 1000 * 2;
	//	
	//	dir_.x = dirVec.x * MathUtil::DegreeToRadian(angle_.x);
	//	//dir_.z = dirVec.z * MathUtil::DegreeToRadian(angle_.z);
	//	dir_ *= rotSpeed_;
	//}
	//


	////cameraVecRot_.y = atan2f(dirVec.x, dirVec.z);
	//worldTransform_.translation += dir_;

	// 行列を初期化
	worldTransform_.matWorld_.identity();

	// 行列を計算
	worldTransform_.UpdateMatrix();

}
