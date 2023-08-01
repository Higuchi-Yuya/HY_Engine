#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "ImGuiManager.h"
class GameCamera
{
public:// �����o�֐�
	void Initialize(const WorldTransform* worldTransform);

	void Update();

	ViewProjection& GetView() { return viewProjection_; }

	void SetCameraPos(Vector3 pos) { playerPos_ = pos; }

private:// �v���C�x�[�g�����o�֐�
	void RotUpdate();

private:// �����o�ϐ�

	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	//�r���[�v���W�F�N�V����
	ViewProjection viewProjection_;
	// �����_��Y���W��ύX����l
	Vector3 offSet;


	// ��]
	Vector3 angle_;
	Vector3 cameraVecRot_;
	Vector3 dir_;
	Vector3 rotSpeed_ = { 5,5,5 };

	// �����_�ƂȂ�v���C���[�̍��W
	Vector3 playerPos_;
};

