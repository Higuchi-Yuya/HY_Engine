#include "GameCamera.h"
#include "JoyPadInput.h"
#include "MathUtil.h"

void GameCamera::Initialize()
{
	viewProjection_.Initialize();
	worldTransform_.Initialize();
	
	titleCameraFPos_= { -0.8f,5.0f,-65 };
	titleCameraFTargetPos_= { -0.8f,2,0 };

	viewProjection_.eye = titleCameraFPos_;
	viewProjection_.target = titleCameraFTargetPos_;

	worldTransform_.translation = { -15,15,-15 };
	worldTransform_.UpdateMatrix();

	offSet = { 0,5,0 };
	
	titleCameraState_ = titleCameraState_;

	doorOpenTimeLimit_ = 60 * 3.5f;
}

void GameCamera::TitleUpdate()
{
	// イージングの処理
	if (IsCanEase_ == true) {
		switch (titleCameraState_)
		{
		// カメラの座標を前に移動
		case GameCamera::FirstMove:
		{
			
			ease_.SetEaseLimitTime(180);

			// イージングの最初のポジション
			Vector3 fEyePos = titleCameraFPos_;
			Vector3 fTargetPos = titleCameraFTargetPos_;

			// イージングの終わりのポジション
			Vector3 eEyePos = { -0.8f,6,-20 };
			Vector3 eTargetPos = { -0.8f,4,0 };

			// カメラのポジションをイージングさせる
			viewProjection_.eye = ease_.OutVec3(fEyePos, eEyePos);
			viewProjection_.target = ease_.OutVec3(fTargetPos, eTargetPos);
			ease_.Update();

			// イージングが終了したら
			if (ease_.GetIsEnd()==true) {
				easeTimer_ = 0;
				titleCameraState_ = SecondMove;
				ease_.Reset();
				IsEaseEnd_ = true;
			}
			break;
		}
		case GameCamera::SecondMove:


			break;
		default:
			break;
		}
	}
	
	
	ImGui::Begin("gameCamera");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));

	//ImGui::InputFloat2("joySrick", &joyStickInfo.x, "%.2f");
	ImGui::InputFloat("ainfo", &aInfo);
	ImGui::InputFloat3("angle", &angle_.x);
	ImGui::InputFloat3("rotPos", &rotPos.x);
	ImGui::InputFloat3("rotnorm", &rotNorm.x);
	ImGui::InputFloat3("rotnorm", &vel.x);
	ImGui::InputFloat("chagePos Y", &offSet.y);
	ImGui::InputFloat3("worldRot", &worldTransform_.rotation.x, "%.2f");
	ImGui::InputFloat3("worldPos", &worldTransform_.translation.x, "%.2f");
	ImGui::InputFloat3("cameraEye", &viewProjection_.eye.x, "%.2f");
	ImGui::InputFloat3("cameraTraget", &viewProjection_.target.x, "%.2f");

	ImGui::End();
	// ビューの更新処理
	viewProjection_.UpdateMatrix();
}

void GameCamera::GameUpdate()
{
	// カメラ回転
	RotUpdate();

	//ビュープロジェクションを更新
	viewProjection_.UpdateMatrix();

	ImGui::Begin("gameCamera");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));

	//ImGui::InputFloat2("joySrick", &joyStickInfo.x, "%.2f");
	ImGui::InputFloat("ainfo", &aInfo);
	ImGui::InputFloat3("angle", &angle_.x);
	ImGui::InputFloat3("rotPos", &rotPos.x);
	ImGui::InputFloat3("rotnorm", &rotNorm.x);
	ImGui::InputFloat3("rotnorm", &vel.x);
	ImGui::InputFloat("chagePos Y", &offSet.y);
	ImGui::InputFloat3("worldRot", &worldTransform_.rotation.x, "%.2f");
	ImGui::InputFloat3("worldPos", &worldTransform_.translation.x, "%.2f");
	ImGui::InputFloat3("cameraEye", &viewProjection_.eye.x, "%.2f");
	ImGui::InputFloat3("cameraTraget", &viewProjection_.target.x, "%.2f");

	ImGui::End();
}

void GameCamera::Reset()
{
	titleCameraState_ = FirstMove;
	IsCanEase_ = false;
	IsEaseEnd_ = false;

	doorOpenTimer_ = 0;
	IsDoorOpen_ = false;
	IsFinshDoor_ = false;

	viewProjection_.eye = titleCameraFPos_;
	viewProjection_.target = titleCameraFTargetPos_;
}

void GameCamera::SetCameraFPos(Vector3 pos)
{
	playerPos_ = pos;
	dirVec = Vector3{0,0,0} - cameraFPos;
	velLength = dirVec.length();
}

void GameCamera::SetCameraPos(Vector3 pos)
{
	playerPos_ = pos; 
}

void GameCamera::SetIsCanEase(bool IsCanEase)
{
	IsCanEase_ = IsCanEase;
}

void GameCamera::SetIsDoorOpen(bool isDoorOpen)
{
	IsDoorOpen_ = isDoorOpen;
}

void GameCamera::RotUpdate()
{
	const float cameraEaseSpeed = 0.1f;//1e-100f;

	// ドアが開いてる間のカメラ処理
	if (IsDoorOpen_ == true &&
		IsFinshDoor_ == false) {
		if (doorOpenTimer_ <= doorOpenTimeLimit_) {
			doorOpenTimer_++;
		}
		else {
			IsFinshDoor_ = true;
		}
		viewProjection_.eye = doorOpenEye_;
		viewProjection_.target = doorOpenTarget_;
	}
	// それ以外の時のカメラ処理
	else {
		viewProjection_.target += ((Vector3(playerPos_.x, playerPos_.y + offSet.y, playerPos_.z)) - viewProjection_.target) * cameraEaseSpeed;
		viewProjection_.eye = ((viewProjection_.target + cameraFPos));// - viewProjection_.eye)* cameraEaseSpeed;
	}

	// 行列を初期化
	worldTransform_.matWorld_.identity();

	// 行列を計算
	worldTransform_.UpdateMatrix();
}
