#pragma once
#include "Object3d.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Quaternion.h"
#include "Light.h"
#include "Input.h"
class Player
{
public:
	~Player();

	// ������
	void Initialize();
	// �X�V����
	void Update();
	// �`�揈��
	void Draw(ViewProjection *view);
private:
	// �C���v�b�g
	Input* input = nullptr;

	// ���f��
	Model* model = nullptr;

	// �I�u�W�F�N�g
	Object3d* playerObj = nullptr;

};

