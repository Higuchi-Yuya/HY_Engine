#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
#include "Player.h"
#include <ParticleManager.h>

class Enemy:public Object3d
{
private:// �T�u�N���X
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

	// ���݂̏��
	State nowState_ = State::Spawn;

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
	int particleNum = 5;

#pragma endregion
};

