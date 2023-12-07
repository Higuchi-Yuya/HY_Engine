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

void GameCamera::RotUpdate()
{
	// Rスティックでカメラ回転
	if (JoypadInput::GetStick(PadCode::RightStick).x > 450) {
		if (aInfo >= 360) {
			aInfo = 0;
			angle_ = { 0,0,0 };
		}
		aInfo += 2;
		angle_.x = MathUtil::DegreeToRadian(aInfo);
		angle_.z = MathUtil::DegreeToRadian(aInfo);
		rotPos.x = viewProjection_.target.x + cosf(angle_.x) * velLength;
		rotPos.y = 15;
		rotPos.z = viewProjection_.target.z + sinf(angle_.z) * velLength;
		rotNorm = rotPos.normalize();
		vel = rotPos.normalize();
		vel.y = 15;
	}

	const float cameraEaseSpeed = 0.1f;//1e-100f;

	
	//vel.y = 15;
	viewProjection_.target += ((Vector3(playerPos_.x, playerPos_.y + offSet.y, playerPos_.z)) - viewProjection_.target) * cameraEaseSpeed;
	viewProjection_.eye = ((viewProjection_.target + cameraFPos));// - viewProjection_.eye)* cameraEaseSpeed;
	//viewProjection_.eye = rotPos;
	// プレイヤーの座標を注視点にセット
	//viewProjection_.target = playerPos_;
	// 方向ベクトルを求める



	if (JoypadInput::GetStick(PadCode::RightStick).x < -450) {

	}


	////cameraVecRot_.y = atan2f(dirVec.x, dirVec.z);
	//worldTransform_.translation += dir_;

	// 行列を初期化
	worldTransform_.matWorld_.identity();

	// 行列を計算
	worldTransform_.UpdateMatrix();

}
