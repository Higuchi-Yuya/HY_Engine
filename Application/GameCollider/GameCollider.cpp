#include "GameCollider.h"
#include "Collision.h"

void GameCollider::Initialize()
{
	// �p�[�e�B�N���̃e�N�X�`���̏�����
	particleTex_.reset(TextureManager::Load2DTextureP("effect.png"));
	// �p�[�e�B�N���̃}�l�[�W���̏�����
	particleMan_ = std::make_unique<ParticleManager>();
	particleMan_->Initialize();
	particleMan_->SetTextureHandle(particleTex_.get());
}

void GameCollider::Updata()
{
	// �����蔻��i�G�l�~�[���̔����j
	for (auto& e : enemys_){
		if (Collision::CheckOBB(player_->worldTransform_, e->worldTransform_)) {
			// �G�l�~�[�����Ԃ�����i���j
			e->worldTransform_.color = { 1,0,0,1 }; 

			// �v���C���[�̃q�b�g�t���O�𗧂Ă�
			isPlayerHit = true;
			
		}
		else {
			e->worldTransform_.color = { 1,1,1,1 };
		}
	}

	
	if (isPlayerHit == true) {
		// �q�b�g���Ɉ�x�p�[�e�B�N�����o��
		if (isPartile == false) {
			OnColParticle();
			isPartile = true;
		}

	}
	// �p�[�e�B�N�����o�����ƃv���C���[�̖��G���Ԃ̃^�C�}�[��i�߂�
	if (isPartile == true) {
		playerHitTimer++;

		// ���G���Ԃ��߂�����v���C���[�̓����蔻����ĂтƂ��悤�ɂ���
		if (playerHitTimer > playerHitTimeMax) {
			playerHitTimer = 0;
			isPartile = false;
			isPlayerHit = false;
		}
	}

	// �p�[�e�B�N���̍X�V����
	particleMan_->Update();
}

void GameCollider::OnColParticle()
{
	// �����蔻��i�v���C���[���̔����j
	for (int i = 0; i < particleNum; i++) {
		// X,Y,Z�S��{-5.0f,+5.0f}�Ń����_���ɕ��z
		//const float md_pos = 0.0f;
		Vector3 pos{};
		//pos.x = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;
		//pos.y = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f + 5;
		//pos.z = (float)rand() / RAND_MAX * md_pos - md_pos / 2.0f;

		// �|�W�V�������v���C���[�̒��S���W�ɃZ�b�g
		pos = player_->worldTransform_.translation;
		pos.y += 0.5f;
		// X,Y,Z�S��{-0.05f,+0.05f}�Ń����_���ɕ��z
		const float md_vel = 0.1f;
		Vector3 vel{};
		vel.x = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * md_vel - md_vel / 2.0f;

		// �d�͂Ɍ����Ă�Y�̂�{-0.001f,0}�Ń����_���ɕ��z
		Vector3 acc{};
		const float md_acc = 0.005f;
		acc.y = -(float)rand() / RAND_MAX * md_acc;

		// �ǉ�
		particleMan_->Add(ParticleManager::Type::Normal, 120, pos, vel, acc, 0.2f, 0.0f, startColor_, endColor_);
	}
}

void GameCollider::Draw(ID3D12GraphicsCommandList* commandList, ViewProjection* viewProjection)
{
	// ----------------�p�[�e�B�N���̕`��͂�������--------------- //
	ParticleManager::PreDraw(commandList);
	particleMan_->Draw(*viewProjection);


	ParticleManager::PostDraw();
	// ----------------�p�[�e�B�N���̕`�悱���܂�----------------- //
}

void GameCollider::AddEnemy(Enemy* enemy)
{
	enemys_.push_back(enemy);
}

void GameCollider::SetPlayer(Player* player)
{
	player_ = player;
}
