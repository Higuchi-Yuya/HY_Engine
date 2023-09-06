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

	boxModel_.reset(Model::LoadFromOBJ("box1x1x1"));

	for (size_t i = 0; i < 4; i++)
	{
		box_[i].reset(Object3d::Create());
		box_[i]->SetModel(boxModel_.get());
		box_[i]->worldTransform_.translation.y = 1;
		switch (i)
		{
		case 0:
			box_[i]->worldTransform_.translation.z = 30 + 50.22f;
			box_[i]->worldTransform_.scale.x = 50;
			box_[i]->worldTransform_.scale.z = 50;
			break;
		case 1:
			box_[i]->worldTransform_.translation.z = -30 - 50.22f;
			box_[i]->worldTransform_.scale.x = 50;
			box_[i]->worldTransform_.scale.z = 50;
			break;
		case 2:
			box_[i]->worldTransform_.translation.x = 30 + 45.8f;
			box_[i]->worldTransform_.scale.x = 30;
			box_[i]->worldTransform_.scale.z = 30;
			break;
		case 3:
			box_[i]->worldTransform_.translation.x = -30 - 47.3f;
			box_[i]->worldTransform_.scale.x = 30;
			box_[i]->worldTransform_.scale.z = 30;
			break;
		default:
			break;
		}
		box_[i]->worldTransform_.UpdateMatrix();
	}



}

void GameCollider::Updata()
{
	//�������s�����G�̏���S�폜
	enemysInfo_.erase(std::remove_if(enemysInfo_.begin(), enemysInfo_.end(), [](Enemy* x) {
		return x->GetAlive() == false;
		}), enemysInfo_.end());

	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player_->GetBullets();

	// �v���C���[�̏����X�t�B�A�̂��̂ɓo�^
	Sphere p;
	p.center = player_->worldTransform_.translation;
	p.center.y += 1.0f;
	p.radius = 1.0f;

	// �����蔻��i�G�l�~�[���̔����j
	for (auto& e : enemysInfo_) {
		// �G�l�~�[�̏����X�t�B�A�̂��̂ɓo�^
		Sphere sphereE;
		sphereE.center = e->worldTransform_.translation;
		sphereE.radius = 1.0f;

		if (e->GetState() == Enemy::State::Alive && Collision::CheckOBB(player_->worldTransform_, e->worldTransform_)) {
			// �G�l�~�[�����Ԃ�����i���j
			e->worldTransform_.color = { 1,0,0,1 };
			//e->SetAlive(false);
			// �v���C���[�̃q�b�g�t���O�𗧂Ă�
			isPlayerHit = true;

		}
		else {
			player_->OnColUpSpeed();
			e->worldTransform_.color = { 1,1,1,1 };
		}

		// -------------�G�ƃv���C���[�̒e�̓����蔻��------------ //
		for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {

			// �v���C���[�̒e�̏����X�t�B�A�̂��̂ɓo�^
			Sphere pB;
			pB.center = playerbullet->worldTransform_.translation;
			pB.radius = 1.0f;

			// �v���C���[�̒e�ƃG�l�~�[�̓����蔻��
			if (e->GetState() == Enemy::State::Alive && Collision::CheckSphere2Sphere(sphereE, pB)) {
				// �G�l�~�[�����S
				e->OnCollision();
				// �e������
				playerbullet->OnCollision();
			}
		}

		// ----------�G�ƓG���m�̓����蔻��-------------- //
		for (auto& e2 : enemysInfo_) {
			if (e2 != e) {
				Sphere sphereE2;
				sphereE2.center = e2->worldTransform_.translation;
				sphereE2.radius = 1.0f;

				if (e->GetState() == Enemy::State::Alive && Collision::CheckSphere2Sphere(sphereE, sphereE2, &e->interPos, &e->rejectVec)) {
					e->pushBackOnCol();
				}
			}
		}


		// -----------�v���C���[�ƓG�̓����蔻��------------- //
		if (e->GetState() == Enemy::State::Alive && Collision::CheckSphere2Sphere(sphereE, p, &e->interPos, &e->rejectVec)) {
			e->pushBackOnCol();

			// �v���C���[�̑O�������X�t�B�A�̂��̂ɓo�^
			Sphere frontP;
			frontP.center = player_->GetFrontPos();
			//frontP.center.y += 1.0f;
			frontP.radius = 0.8f;
			if (Collision::CheckSphere2Sphere(sphereE, frontP)) {
				player_->OnColDownSpeed();
			}
			else {
				player_->OnColUpSpeed();
			}
		}

		// -----------�G�ƃt�B�[���h�̃I�u�W�F�N�g�̓����蔻��------------- //
		for (auto obj : objectsInfo_) {
			Sphere sphereE2;
			sphereE2.center = obj->worldTransform_.translation;
			sphereE2.center.y += 1;
			sphereE2.radius = 1.0f;

			if (e->GetState() == Enemy::State::Alive && Collision::CheckSphere2Sphere(sphereE, sphereE2, &e->interPos, &e->rejectVec)) {
				e->pushBackOnCol();
			}
		}
	}

	// �t�B�[���h�̃I�u�W�F�N�g�̓����蔻��
	for (auto obj : objectsInfo_) {
		Sphere sphereObj;
		sphereObj.center = obj->worldTransform_.translation;
		sphereObj.center.y = 1.0f;
		sphereObj.radius = 0.6f;

		// -----------�v���C���[�ƃt�B�[���h�̃I�u�W�F�N�g�̓����蔻��----------- //
		if (Collision::CheckSphere2Sphere(p, sphereObj, &player_->interPos, &player_->rejectVec)) {
			player_->pushBackOnCol();
		}

		// -------------�v���C���[�̒e�ƃt�B�[���h�̃I�u�W�F�N�g�̓����蔻��------------ //
		for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {

			// �v���C���[�̒e�̏����X�t�B�A�̂��̂ɓo�^
			Sphere pB;
			pB.center = playerbullet->worldTransform_.translation;
			pB.radius = playerbullet->worldTransform_.scale.x;

			// �v���C���[�̒e�ƃG�l�~�[�̓����蔻��
			if (Collision::CheckSphere2Sphere(sphereObj, pB)) {
				// �e������
				playerbullet->OnCollision();
			}
		}
	}

	// �ǂƂ̓����蔻��
	for (size_t i = 0; i < 4; i++)
	{
		// ------------�v���C���[�ƕǂ̓����蔻��---------- //
		if (Collision::CheckSphere2AABB(p, box_[i]->worldTransform_, &player_->interPos, &player_->rejectVec)) {
			player_->pushBackOnCol();
		}

		// ------------�G�ƕǂ̓����蔻��----------- //
		for (auto& e : enemysInfo_) {
			// �G�l�~�[�̏����X�t�B�A�̂��̂ɓo�^
			Sphere sphereE;
			sphereE.center = e->worldTransform_.translation;
			sphereE.radius = 1.0f;

			if (Collision::CheckSphere2AABB(sphereE, box_[i]->worldTransform_, &e->interPos, &e->rejectVec)) {
				box_[i]->worldTransform_.color = { 1,0,0,1 };
				e->pushBackOnCol();
			}
		}

		// --------�v���C���[�̒e�ƕǂ̓����蔻��---------- //
		for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {

			// �v���C���[�̒e�̏����X�t�B�A�̂��̂ɓo�^
			Sphere pB;
			pB.center = playerbullet->worldTransform_.translation;
			pB.radius = 1.0f;

			if (Collision::CheckSphere2AABB(pB, box_[i]->worldTransform_)) {
				// �e������
				playerbullet->OnCollision();
			}
		}

		box_[i]->Update();
	}

	if (isPlayerHit == true) {
		// �q�b�g���Ɉ�x�p�[�e�B�N�����o��
		if (isPartile == false) {
			OnColParticle();
			player_->OnColHitPoint();
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
		const float md_acc = 0.003f;
		acc.y = -(float)rand() / RAND_MAX * md_acc;

		Vector3 angle = { 0,0,0 };
		// �ǉ�
		particleMan_->Add(ParticleManager::Type::Normal, 120, pos, vel, acc, angle, 0.2f, 0.0f, startColor_, endColor_);
	}
}

void GameCollider::Draw(ID3D12GraphicsCommandList* commandList, ViewProjection* viewProjection)
{
	// ----------------�p�[�e�B�N���̕`��͂�������--------------- //

	particleMan_->Draw(*viewProjection);



	// ----------------�p�[�e�B�N���̕`�悱���܂�----------------- //
}

void GameCollider::Draw3D(ViewProjection* viewProjection)
{
	for (size_t i = 0; i < 4; i++)
	{
		//box_[i]->Draw(viewProjection);
	}
}

void GameCollider::AddEnemy(Enemy* enemy)
{
	enemysInfo_.push_back(enemy);
}

void GameCollider::AddObj(Object3d* obj)
{
	objectsInfo_.push_back(obj);
}

void GameCollider::SetPlayer(Player* player)
{
	player_ = player;
}

void GameCollider::Reset()
{
	enemysInfo_.clear();
}
