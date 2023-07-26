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
	// �s���������
	worldTransform_.matWorld_.identity();

	// �s����v�Z
	worldTransform_.matWorld_.rotation(worldTransform_.rotation);
	worldTransform_.matWorld_.translate(worldTransform_.translation);

	// �e�̍s����|����
	worldTransform_.matWorld_ *= worldTransform_.parent_->matWorld_;

	// ���[���h�s��̏ꏊ���r���[�Ɏ��[
	viewProjection_.eye = Vector3(worldTransform_.matWorld_.m[3][0], worldTransform_.matWorld_.m[3][1], worldTransform_.matWorld_.m[3][2]);
	
	// �v���C���[�̍��W�𒍎��_�ɃZ�b�g
	viewProjection_.target = Vector3(playerPos_.x, playerPos_.y + 3, playerPos_.z);


	//�r���[�v���W�F�N�V�������X�V
	viewProjection_.UpdateMatrix();

	ImGui::Begin("gameCamera");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));

	//ImGui::InputFloat2("joySrick", &joyStickInfo.x, "%.2f");
	ImGui::InputFloat3("cameraEye", &viewProjection_.eye.x, "%.2f");
	ImGui::InputFloat3("cameraTraget", &viewProjection_.target.x, "%.2f");

	ImGui::End();
}
