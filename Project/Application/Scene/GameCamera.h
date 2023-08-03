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

	void SetCameraFPos(Vector3 pos);

	void SetCameraPos(Vector3 pos);

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
	float aInfo;
	Vector3 angle_;
	Vector3 cameraVecRot_;
	Vector3 dirVec;
	float velLength;
	const Vector3 cameraFPos = { -15,5,-15 };;
	Vector3 rotPos= { -15,15,-15 };
	Vector3 rotNorm;
	Vector3 vel;
	Vector3 rotSpeed_ = { 5,5,5 };

	// �����_�ƂȂ�v���C���[�̍��W
	Vector3 playerPos_;
};

