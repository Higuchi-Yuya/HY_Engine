#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
#include "GameCamera.h"
#include "PlayerBullet.h"
#include "Sprite.h"

class Player:public Object3d
{
public:
	// 3D�I�u�W�F�N�g����
	static Player* Create(Model* model = nullptr);

public:

	// �f�X�g���N�^
	~Player();

	// ������
	bool Initialize()override;

	// ���t���[������
	void Update()override;

	// �`��
	void Draw(ViewProjection* view)override;

	// �v���C���[��2D�`��O��
	void Draw2DFront();

	// ���Z�b�g�֐�
	void Reset();

public:// �Q�b�^�[

	// �|�W�V�����̃Q�b�^�[
	const Vector3 GetWorldPosition ()const;

	// �J�����p�̃��[���h�g�����X�t�H�[���̎擾
	const WorldTransform *GetCameraWorld()const;

	// �v���C���[�O���ɂ��郏�[���h�g�����X�t�H�[���̎擾
	const Vector3 GetFrontPos()const;

	// �v���C���[�̐����Ă���t���O�̎擾
	const bool GetIsAlive()const;

public:// �Z�b�^�[

	// ���[���h�g�����X�t�H�[���̏����Z�b�g
	void SetWorldTransInfo(WorldTransform worldTrans);

	// �Q�[���J�������Z�b�g
	void SetGameCamera(GameCamera* gameCamera);

	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets();

public:// �����蔻��

	// �Փˎ��R�[���o�b�N�֐�
	void OnCollision();

	// �����߂��Փ˔���R�[���o�b�N�֐�
	void pushBackOnCol();

	// �O���ɂ���G�̑̂Ɠ��������ꍇ�Ɉړ����x��������
	void OnColDownSpeed();

	// �O���ɂ���G�̑̂Ɠ������Ă��Ȃ��ꍇ�Ɉړ����x���グ��
	void OnColUpSpeed();

	/// <summary>
	/// �����蔻��q�b�g�|�C���g
	/// </summary>
	void OnColHitPoint();

private:// �v���C�x�[�g�֐�
	/// <summary>
	/// �ړ��̍X�V����
	/// </summary>
	void MoveUpdate();

	/// <summary>
	/// �U������
	/// </summary>
	void Attack();

public:// �p�u���b�N�ϐ�
	// �Փ˓_
	Vector3 interPos;
	// �r�˃x�N�g��
	Vector3 rejectVec;


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
	float moveSpeedMax_ = 0.2f;
	float moveSpeedMin_ = 0.05f;

	// �؂�Ă���J����
	GameCamera *bGameCamera;

	// �e�֘A
	// �O�ɂ��鉼�̃��[���h�g�����X�t�H�[��
	WorldTransform frontW_;

	//���f��
	std::unique_ptr<Model> bulletModel_ = nullptr;

	// �e�̃C���^�[�o��
	float bulletInterval = 20;
	float bulletInterTimer = 0;

	// �����Ă��邩�t���O
	bool IsAlive_ = true;

#pragma region �v���C���[��HP�֘A
	// �v���C���[��HP�֘A
	float playerHitPoint_ = 0;
	float playerHitPointMax_ = 100;

	// �v���C���[��HP�̃X�v���C�g
	std::unique_ptr<Sprite> playerHpBar_ = nullptr;
	std::unique_ptr<Sprite> playerHpInside_ = nullptr;

	// �v���C���[��Hp�̃e�N�X�`���n���h��
	std::unique_ptr<Texture> textureHandleHpBar_ = nullptr;
	std::unique_ptr<Texture> textureHandleHpInside_ = nullptr;

	Vector2 pHpBarPos_ = { 20,50 };
	Vector2 pHpInsidePos_ = { 20,50 };

	Vector2 pHpBarSize_ = { 500,70 };
	Vector2 pHpInsideSize_ = { 500,70 };
#pragma endregion

};

