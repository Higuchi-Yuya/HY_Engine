#include "Enemy.h"
#include "Collision.h"
#include "MathUtil.h"
#include "Easing.h"

std::unique_ptr<Texture> Enemy::sParticleTex_ = nullptr;

void Enemy::StaticInitialize()
{
	// �p�[�e�B�N���̃e�N�X�`���̏�����
	sParticleTex_.reset(TextureManager::Load2DTextureP("effect.png"));

}

void Enemy::StaticFinalize()
{
	sParticleTex_ = nullptr;
}

void Enemy::Initialize(Model* model, Player* player)
{
	player_ = player;

	Object3d::Initialize();
	// ���f���̃Z�b�g
	if (model) {
		SetModel(model);
	}

	IsAlive_ = true;
	worldTransform_.translation.y -= 2.5f;
	//worldTransform_.rotation.y += MathUtil::DegreeToRadian(90);
	//worldTransform_.scale.x = 1.0f;
	worldTransform_.UpdateMatrix();

	// �p�[�e�B�N���̃}�l�[�W���̏�����
	particleMan_ = std::make_unique<ParticleManager>();
	particleMan_->Initialize();
	particleMan_->SetTextureHandle(sParticleTex_.get());
}

void Enemy::Update()
{
	switch (nowState_)
	{
	case Enemy::State::Spawn:
		// �X�|�[���̍ŏ��̃|�W�V�������Z�b�g���ꂽ��
		if (spawnFirstPos_ != 0) {
			spawnTimer++;

			spawnTimeLate = spawnTimer / spawnTimeMax;
			float spawnPy = (float)Easing::In(spawnFirstPos_.y, spawnEndPos_.y, spawnTimer, spawnTimeMax);
			worldTransform_.translation.y = spawnPy;
			//worldTransform_.translation = spawnEndPos_;
			ParticleUpdate();

			// �X�|�[�����I�������
			if (spawnTimer > spawnTimeMax) {
				nowState_ = State::Alive;
			}
		}
		else {
			// �ŏ��̃X�|�[���|�W�V�����̃Z�b�g
			spawnFirstPos_ = worldTransform_.translation;
			spawnEndPos_ = spawnFirstPos_;
			spawnEndPos_.y += 2.5f;
		}
		break;
	case Enemy::State::Alive:
		{
			timer++;
			followTimer++;
			if (timer > maxTime) {
				timer = 0;
			}
			// �؂�Ă����v���C���[�̃|�W�V����
			Vector3 playerPos = player_->worldTransform_.translation;
			if (followTimer > followTimeMax) {
				// �Ǐ]�̃x�N�g���̍X�V
				Vector3 velo = playerPos - worldTransform_.translation;
				followVec = velo.normalize() * followSpeed;
				followTimer = 0;
			}

			worldTransform_.translation += followVec;
			worldTransform_.rotation.y += MathUtil::DegreeToRadian(5);

			OnCollision();

			// ���񂾂��Ԃ�ύX
			if (IsAlive_ == false) {
				nowState_ = State::Dead;
			}
		}
		
		break;
	case Enemy::State::Dead:
		disoTimer_++;
		disoTimeLate_ = disoTimer_ / disoTimeMax_;
		dissolve_.isActiveDissolve_ = true;

		dissolve_.dissolveTime_ = disoTimeLate_;
		break;
	default:
		break;
	}

	particleMan_->Update();
	// �s��̍X�V�Ȃ�
	Object3d::Update();
}

void Enemy::ParticleUpdate()
{
	if (spawnTimer < spawnTimeMax - 30) {
		// �X�|�[������Ƃ��̃p�[�e�B�N��
		for (int i = 0; i < particleNum; i++) {

			Vector3 pos{};
			// �|�W�V�������G�l�~�[�̒��S���W�ɃZ�b�g
			pos = spawnFirstPos_;
			pos.y += 1.5f;

			// X,Y,Z�S�ĂŃ����_���ɕ��z
			const float md_vel = 0.1f;
			Vector3 vel{};

			const float md_angle = 360.0f;
			Vector3 angle{};
			float randAngle = (float)rand() / RAND_MAX * md_angle - md_angle;
			angle.x = randAngle;
			angle.z = randAngle;
			angle.x += MathUtil::DegreeToRadian(angle.x);
			angle.z += MathUtil::DegreeToRadian(angle.z);
			// �p�x�̕����Ƀ|�W�V�������Z�b�g
			pos.x = worldTransform_.translation.x + cos(angle.x) * 1.0f;
			pos.z = worldTransform_.translation.z + sin(angle.z) * 1.0f;

			// �d�͂Ɍ����Ă�Y�݂̂Ń����_���ɕ��z
			Vector3 acc{};
			const float md_acc = 0.0015f;
			acc.y = (float)rand() / RAND_MAX * md_acc;

			// �ǉ�
			particleMan_->Add(ParticleManager::Type::EnemySpawn, 120, pos, vel, acc, angle, 0.2f, 0.0f, startColor_, endColor_);
		}
	}
}

void Enemy::Draw(ViewProjection* view)
{
	Object3d::Draw(view);
	
}

void Enemy::DrawParticle(ViewProjection* view)
{
	particleMan_->Draw(*view);
}

void Enemy::OnCollision()
{

}

void Enemy::SetWorldTransInfo(WorldTransform worldTrans)
{
	worldTransform_.translation = worldTrans.translation;
	worldTransform_.scale = worldTrans.scale;
	worldTransform_.rotation = worldTrans.rotation;
}
