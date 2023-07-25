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

	worldTransform_.translation.y += 1.0f;
	worldTransform_.rotation.y += MathUtil::DegreeToRadian(90);
	worldTransform_.scale.x = 2.0f;
	worldTransform_.UpdateMatrix();
}

void Enemy::Update()
{
	timer++;
	if (timer > maxTime) {
		timer = 0;
	}
	worldTransform_.translation.x = MathUtil::Sin_ZeroToOne(0.0f, maxTime, timer, 3.2f);
	worldTransform_.rotation.y += MathUtil::DegreeToRadian(5);
	OnCollision();

	// �s��̍X�V�Ȃ�
	Object3d::Update();
}

void Enemy::Draw(ViewProjection* view)
{
	Object3d::Draw(view);
}

void Enemy::OnCollision()
{
	//Sphere enemyS;
	//enemyS.center = { worldTransform_.translation };
	//enemyS.radius = 1.0f;

	//Sphere playerS;
	//playerS.center = player_->worldTransform_.translation;
	//playerS.radius = 1.0f;


}