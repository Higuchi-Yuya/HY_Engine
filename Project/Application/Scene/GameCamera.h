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


private:// �����o�ϐ�

	//���[���h�ϊ��f�[�^
	WorldTransform worldTransform_;
	//�r���[�v���W�F�N�V����
	ViewProjection viewProjection_;

	Vector3 playerPos_;
};

