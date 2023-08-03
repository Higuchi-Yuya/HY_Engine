#pragma once
#include "Enemy.h"
#include "Player.h"
#include "ParticleManager.h"

class GameCollider
{
public:// �����o�֐�
	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �X�V����
	/// </summary>
	void Updata();

	/// <summary>
	/// �G�l�~�[�Ɠ���������v���C���[�̒��S�Ƀp�[�e�B�N�����o������
	/// </summary>
	void OnColParticle();

	/// <summary>
	/// �`�揈��
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* commandList, ViewProjection* viewProjection);

	/// <summary>
	/// �G�l�~�[�̏����x�N�^�[�Ɋi�[
	/// </summary>
	/// <param name="enemy">�G�l�~�[</param>
	void AddEnemy(Enemy* enemy);

	/// <summary>
	/// �v���C���[���擾����
	/// </summary>
	/// <param name="player">�v���C���[</param>
	void SetPlayer(Player* player);

private:// �v���C�x�[�g�֐�


private:// �����o�ϐ�

	int playerHitTimer = 0;
	int playerHitTimeMax = 60;
	bool isPlayerHit = false;
	bool isPartile = false;

	std::vector<Enemy*>enemysInfo_;
	Player* player_ = nullptr;

#pragma region �p�[�e�B�N��
	// �p�[�e�B�N���̃e�N�X�`��
	std::unique_ptr<Texture> particleTex_;
	// �p�[�e�B�N���}�l�[�W��
	std::unique_ptr<ParticleManager> particleMan_;

	// �p�[�e�B�N���̐F
	// �F�̏����l
	Vector4 startColor_ = { 1,0,0,1 };
	// �F�̍ŏI�l
	Vector4 endColor_ = { 1,1,1,0 };
	// ��x�ɂ����p�[�e�B�N���̐�
	int particleNum = 50;

#pragma endregion

};

