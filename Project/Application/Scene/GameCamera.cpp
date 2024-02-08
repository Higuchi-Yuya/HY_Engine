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
				ease_.Reset();
				IsEaseEnd_ = true;
				titleCameraState_ = MoveEnd;
			}
			break;
		}
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
	switch (gameCameraState_)
	{
	case GameCamera::FirstEvent:
		GameFirstEventUpdate();
		break;
	case GameCamera::Normal:
		// カメラ回転
		RotUpdate();
		break;
	case GameCamera::DoorOpen:
		break;
	default:
		break;
	}

	//ビュープロジェクションを更新
	viewProjection_.UpdateMatrix();

	ImGui::Begin("gameCamera");

	ImGui::SetNextWindowSize(ImVec2(500, 100));

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

void GameCamera::GameFirstEventUpdate()
{
	switch (firstEventState_)
	{
	case GameCamera::EventCamera:// イベントカメラの時の処理
		firstEventTimer++;
		if (firstEventTimer >= firstEventTimeLimit) {
			firstEventTimer = 0;
			firstEventState_ = SetValue;
		}

		viewProjection_.eye = firstEventEye_;
		viewProjection_.target = firstEventTarget_;

		break;
	case GameCamera::SetValue:// カメラがイージングする値を設定

		firstTargetS = (Vector3(playerPos_.x, playerPos_.y + offSet.y, playerPos_.z));
		firstTargetE = { 0,0.5f,1.5f };
		firstEventTimeLimit = 60;
		firstEyeS = firstTargetS + cameraFPos;
		firstEyeE = { firstEyeS.x,firstEyeS.y + 5,-10 };
		firstEventState_ = TipsCameraStart;
		easeFirstCamera.SetEaseLimitTime(120);

		break;
	case GameCamera::TipsCameraStart:// ヒントカメラの行き

		easeFirstCamera.Update();

		viewProjection_.eye = easeFirstCamera.InOut(firstEyeS, firstEyeE);
		viewProjection_.target = easeFirstCamera.InOut(firstTargetS, firstTargetE);

		if (easeFirstCamera.GetIsEnd() == true) {
			firstEventTimer++;
		}
		if (firstEventTimer >= firstEventTimeLimit) {
			firstEventTimer = 0;
			firstEventState_ = TipsCameraEnd;
			easeFirstCamera.Reset();
		}
		break;
	case GameCamera::TipsCameraEnd:// ヒントカメラの戻り

		easeFirstCamera.Update();

		viewProjection_.eye = easeFirstCamera.InOut(firstEyeE, firstEyeS);
		viewProjection_.target = easeFirstCamera.InOut(firstTargetE, firstTargetS);

		// イベントカメラが終了したらゲームカメラの状態を普通の
		// プレイヤーを追従するカメラに変更する
		if (easeFirstCamera.GetIsEnd() == true) {
			gameCameraState_ = Normal;
			IsFirstCameraEnd = true;
		}
		break;
	default:
		break;
	}
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

	gameCameraState_ = FirstEvent;
	firstEventState_ = EventCamera;
	firstEventTimer = 0;
	firstEventTimeLimit = 600;
	easeFirstCamera.Reset();
	IsFirstCameraEnd = false;
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

void GameCamera::SetGameState(GameCameraState state)
{
	gameCameraState_ = state;
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
		viewProjection_.eye = ((viewProjection_.target + cameraFPos));
	}

	// 行列を初期化
	worldTransform_.matWorld_.identity();

	// 行列を計算
	worldTransform_.UpdateMatrix();
}
