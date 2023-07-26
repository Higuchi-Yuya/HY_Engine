#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
#include "Player.h"

class Enemy:public Object3d
{
public:

	// ������
	void Initialize(Model* model,Player* player);

	// ���t���[������
	void Update()override;

	// �`��
	void Draw(ViewProjection* view)override;

	// �Փˎ��R�[���o�b�N�֐�
	void OnCollision();

private:
	// �؂�Ă���v���C���[�̏����ꎞ�I�ɕۑ�
	Player* player_ = nullptr;

	// �����������ǂ���
	bool IsCollision = false;

	// �t���O
	bool isH = false;
	float timer = 0;
	float maxTime = 240;
};

