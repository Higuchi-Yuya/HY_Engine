#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
#include "GameCamera.h"
#include "PlayerBullet.h"
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

	// �J�����p�̃��[���h�g�����X�t�H�[���̎擾
	const WorldTransform *GetCameraWorld()const;

	// ���[���h�g�����X�t�H�[���̏����Z�b�g
	void SetWorldTransInfo(WorldTransform worldTrans);

	// �Q�[���J�������Z�b�g
	void SetGameCamera(GameCamera* gameCamera);

	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets();

private:
	/// <summary>
	/// �ړ��̍X�V����
	/// </summary>
	void MoveUpdate();

	/// <summary>
	/// �U������
	/// </summary>
	void Attack();

private:

	bool IsStickOn = false;

	WorldTransform cameraWorld_;
	float deadZone = 450;
	Vector3 moveValue = { 0.1f,0.0f,0.1f };

	//�v���C���[�֘A
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	// �ڒn�t���O
	bool onGround = true;
	// �����x�N�g��
	Vector3 fallV;

	Vector3 frontVec_ = { 0,0,1 };

	// �ړ��x�N�g��
	Vector3 moveVel_;
	float moveSpeed_ = 0.2f;

	// �؂�Ă���J����
	GameCamera *bGameCamera;

	// �e�֘A
	// �O�ɂ��鉼�̃��[���h�g�����X�t�H�[��
	WorldTransform frontW_;

	//���f��
	std::unique_ptr<Model> bulletModel_ = nullptr;
	// �e�̃C���^�[�o��
	float bulletInterval = 40;
	float bulletInterTimer = 0;

};

