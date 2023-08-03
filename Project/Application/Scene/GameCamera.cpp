#include "GameCamera.h"
#include "JoyPadInput.h"
#include "MathUtil.h"

void GameCamera::Initialize(const WorldTransform* worldTransform)
{
	viewProjection_.Initialize();

	
	worldTransform_.Initialize();
	
	//worldTransform_.parent_ = worldTransform;
	worldTransform_.translation = { -15,15,-15 };
	worldTransform_.UpdateMatrix();

	offSet = { 0,5,0 };
	
}

void GameCamera::Update()
{
	// �J������]
	RotUpdate();

	//�r���[�v���W�F�N�V�������X�V
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

void GameCamera::RotUpdate()
{
	// R�X�e�B�b�N�ŃJ������]
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
	// �v���C���[�̍��W�𒍎��_�ɃZ�b�g
	//viewProjection_.target = playerPos_;
	// �����x�N�g�������߂�



	if (JoypadInput::GetStick(PadCode::RightStick).x < -450) {

	}


	////cameraVecRot_.y = atan2f(dirVec.x, dirVec.z);
	//worldTransform_.translation += dir_;

	// �s���������
	worldTransform_.matWorld_.identity();

	// �s����v�Z
	worldTransform_.UpdateMatrix();

}
