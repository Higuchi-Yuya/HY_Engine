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

	const bool GetAlive() { return IsAlive_; }

	// ���[���h�g�����X�t�H�[���̏����Z�b�g
	void SetWorldTransInfo(WorldTransform worldTrans);

	/// <summary>
	/// �����Ă��邩�t���O�̃Z�b�g
	/// </summary>
	/// <param name="isAlive">�����Ă��邩�t���O</param>
	void SetAlive(bool isAlive) { IsAlive_ = isAlive; }


private:
	// �؂�Ă���v���C���[�̏����ꎞ�I�ɕۑ�
	Player* player_ = nullptr;

	// �����������ǂ���
	bool IsCollision = false;

	// �����Ă��邩�ǂ���
	bool IsAlive_ = false;

	// �f�B�]���u�֘A
	float disoTimer_ = 0;
	float disoTimeMax_ = 60 * 3;
	float disoTimeLate_ = 0;

	// �t���O
	bool isH = false;

	// �Ǐ]�֘A
	float followTimer = 0;
	float followTimeMax = 20;
	Vector3 followVec;
	float followSpeed = 0.1f;

	float timer = 0;
	float maxTime = 240;
};

