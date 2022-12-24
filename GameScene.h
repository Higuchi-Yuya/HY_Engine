#pragma once
#include "Input.h"
#include "Sprite.h"
#include "Object3d.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Quaternion.h"
#include "Light.h"
#include "Player.h"

class GameScene
{
public:
	// �R���X�g���N�^
	GameScene();
	// �f�X�g���N�^
	~GameScene();

	// ����������
	void Initialize();

	// �X�V����
	void Update();

	// �`�揈��
	void Draw2Dback();

	void Draw3D();

	void Draw2Dfront();

private:// �����o�ϐ�

	// �C���v�b�g
	Input* input = nullptr;

	// �I�u�W�F�N�g���ʂ̃��C�g
	Light* light = nullptr;

	// �e�N�X�`���n���h��
	int textureHandle = 0;
	int textureHandle2 = 0;

	// �X�v���C�g
	Sprite* sprite = nullptr;
	Sprite* sprite2 = nullptr;

	// ���f��
	
	Model* model_2 = nullptr;

	// �I�u�W�F�N�g
	Player* player = nullptr;
	Object3d* obj_2 = nullptr;

	// �r���[�v���W�F�N�V����
	ViewProjection* view = nullptr;


};

