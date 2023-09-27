#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
#include "Player.h"
#include <ParticleManager.h>

class Enemy:public Object3d
{
public:// �T�u�N���X
	enum class State
	{
		Spawn,// ����
		Alive,// �����Ă���Ƃ�
		Dead,// ����ł鎞
	};

public:
	/// <summary>
	/// �ÓI������
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// �ÓI���
	/// </summary>
	static void StaticFinalize();

	// ������
	void Initialize(Model* model,Player* player);

	// ���t���[������
	void Update()override;

	// �p�[�e�B�N���̍X�V����
	void ParticleUpdate();

	// �`��
	void Draw(ViewProjection* view)override;

	// �p�[�e�B�N���`��
	void DrawParticle(ViewProjection* view);

	// �Փˎ��R�[���o�b�N�֐�
	void OnCollision();

	// �����߂��Փ˔���R�[���o�b�N�֐�
	void pushBackOnCol();

	// �����Ă��邩���擾
	const bool GetAlive() { return IsAlive_; }

	// ���S���[�V�������S�ďI����Ă��邩���擾
	const bool GetDeadMotionEnd() { return IsDeadMotionEnd; }

	// ���̏�Ԃ��擾
	const State GetState() { return nowState_; }

	// �ړ��O�̃|�W�V�����̎擾
	const Vector3 GetOldPos() { return oldPos_; }

	// ���[���h�g�����X�t�H�[���̏����Z�b�g
	void SetWorldTransInfo(WorldTransform worldTrans);

	// �|�W�V�����̃Z�b�g
	void SetWorldPos(Vector3 pos);

	/// <summary>
	/// �����Ă��邩�t���O�̃Z�b�g
	/// </summary>
	/// <param name="isAlive">�����Ă��邩�t���O</param>
	void SetAlive(bool isAlive) { IsAlive_ = isAlive; }

public:
	// �Փ˓_
	Vector3 interPos;
	// �r�˃x�N�g��
	Vector3 rejectVec;

private:
	// �؂�Ă���v���C���[�̏����ꎞ�I�ɕۑ�
	Player* player_ = nullptr;

	// �����������ǂ���
	bool IsCollision = false;

	// �����Ă��邩�ǂ���
	bool IsAlive_ = false;

	// ���S�����[�V�������I����Ă��邩�ǂ���
	bool IsDeadMotionEnd = false;

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

	// ���݂̏��
	State nowState_ = State::Spawn;

	// ���C�t
	uint32_t nowLife_ = 0;
	const uint32_t maxLife_ = 3;

	// �ړ�����O�̌Â��|�W�V����
	Vector3 oldPos_{};

#pragma region �X�|�[���֘A
	float spawnTimer = 0;
	float spawnTimeMax = 240;
	float spawnTimeLate = 0;
	Vector3 spawnFirstPos_ = { 0,0,0 };
	Vector3 spawnEndPos_;


#pragma endregion


#pragma region �p�[�e�B�N��
	// �p�[�e�B�N���̃e�N�X�`��
	static std::unique_ptr<Texture> sParticleTex_;
	// �p�[�e�B�N���}�l�[�W��
	std::unique_ptr<ParticleManager> particleMan_;

	// �p�[�e�B�N���̐F
	// �F�̏����l
	Vector4 startColor_ = { 1,0.05f,0.05f,1 };
	// �F�̍ŏI�l
	Vector4 endColor_ = { 0,0,0,0 };
	// ��x�ɂ����p�[�e�B�N���̐�
	int particleNum = 15;

#pragma endregion
};

