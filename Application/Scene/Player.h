#pragma once
#include "Object3d.h"
class Player:public Object3d
{
public:
	// 3D�I�u�W�F�N�g����
	static Player* Create(Model* model = nullptr);

public:
	~Player();
	// ������
	bool Initialize()override;

	// ���t���[������
	void Update()override;

	// �`��
	void Draw(ViewProjection* view)override;

	// �Փˎ��R�[���o�b�N�֐�
	void OnCollision(const CollisionInfo& info)override;

private:
	Model* atariModel = nullptr;
	Object3d* atari = nullptr;

	Input* input = new Input;
};

