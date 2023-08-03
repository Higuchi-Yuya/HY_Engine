#include "Enemy.h"
#include "Collision.h"
#include "MathUtil.h"

void Enemy::Initialize(Model* model, Player* player)
{
	player_ = player;

	Object3d::Initialize();
	// ���f���̃Z�b�g
	if (model) {
		SetModel(model);
	}

	IsAlive_ = true;
	worldTransform_.translation.y += 1.0f;
	worldTransform_.rotation.y += MathUtil::DegreeToRadian(90);
	worldTransform_.scale.x = 2.0f;
	worldTransform_.UpdateMatrix();
}

void Enemy::Update()
{
	// �����Ă��鎞
	if (IsAlive_){
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

	}
	else {
		disoTimer_++;
		disoTimeLate_ = disoTimer_ / disoTimeMax_;
		dissolve_.isActiveDissolve_ = true;

		dissolve_.dissolveTime_ = disoTimeLate_;
	}
	
	// �s��̍X�V�Ȃ�
	Object3d::Update();
}

void Enemy::Draw(ViewProjection* view)
{
	Object3d::Draw(view);
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
