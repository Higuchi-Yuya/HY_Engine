#pragma once
#pragma once
#include "Object3d.h"
#include <cassert>

class PlayerBullet:public Object3d
{
public:
	void Initialize(Model* model, const WorldTransform worldTransform, const Vector3& velocity);

	void Update()override;

	void Draw(ViewProjection* view)override;

	bool IsDead() const { return isDead_; }

	// ���[���h�e���W���擾
	Vector3 GetBulletWorldPosition();

	// �Փ˂����o������Ăяo�����R�[���o�b�N�֐�
	void OnCollision();
private:

	//���x
	Vector3 velocity_;

	//����<frm>
	static const int32_t kLifeTime = 60 * 5;

	//�f�X�^�C�}�[
	int32_t deathTimer_ = kLifeTime;

	//�f�X�t���O
	bool isDead_ = false;
};

