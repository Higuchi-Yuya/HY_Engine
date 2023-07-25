#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
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
	void OnCollision(const CollisionInfo& info);

	// �|�W�V�����̃Q�b�^�[
	const Vector3 GetWorldPosition ()const;

private:
	/// <summary>
	/// �ړ��̍X�V����
	/// </summary>
	void MoveUpdate();

private:

	bool IsStickOn = false;

	float deadZone = 450;
	Vector3 moveValue = { 0.1f,0.0f,0.1f };

	// �ڒn�t���O
	bool onGround = true;
	// �����x�N�g��
	Vector3 fallV;
};
