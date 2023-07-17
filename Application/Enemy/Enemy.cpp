#include "Enemy.h"
#include "Collision.h"

void Enemy::Initialize(Model* model, Player* player)
{
	player_ = player;

	Object3d::Initialize();
	// ���f���̃Z�b�g
	if (model) {
		SetModel(model);
	}

	worldTransform_.translation.y += 1.0f;
	worldTransform_.UpdateMatrix();
}

void Enemy::Update()
{

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
	Sphere enemyS;
	enemyS.center = { worldTransform_.translation };
	enemyS.radius = 1.0f;

	Sphere playerS;
	playerS.center = player_->worldTransform_.translation;
	playerS.radius = 1.0f;

	if (Collision::CheckSphere2Sphere(playerS, enemyS)) {
		worldTransform_.color = { 1,0,0,1 };

	}
	else {
		worldTransform_.color = { 1,1,1,1 };
	}
}
