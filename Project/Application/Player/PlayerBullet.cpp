#include "PlayerBullet.h"

void PlayerBullet::Initialize(Model* model, const WorldTransform worldTransform, const Vector3& velocity) {
	//NULL�|�C���^�`�F�b�N
	assert(model);

	SetModel(model);

	//�����Ŏ󂯎�������x�������o�ϐ��ɑ��
	velocity_ = velocity;

	//���[���h�g�����X�t�H�[���̏�����
	Object3d::Initialize();

	//�����Ŏ󂯎�����������W���Z�b�g
	worldTransform_.translation = {
	  worldTransform.matWorld_.m[3][0], worldTransform.matWorld_.m[3][1],
	  worldTransform.matWorld_.m[3][2] };

	worldTransform_.scale.x = 0.6f;
	worldTransform_.scale.y = 0.6f;
	worldTransform_.scale.z = 0.6f;

	// �ł����Ɍ����ăI�u�W�F�N�g����]������

	// Y������p�x(��y)
	worldTransform_.rotation.y = std::atan2(velocity_.x, velocity_.z);
	// Y�������-��y�񂷉�]�s����v�Z
	Matrix4 RotY;
	Matrix4 mathMat;
	RotY.rotateY(-worldTransform_.rotation.y);
	// velosity_�ɉ�]�s����|���Z����velosityZ�����߂�
	Vector3 velosityZ = velocity_;
	velosityZ = mathMat.transformNotW(velosityZ, RotY);

	// X������p�x(��x)
	worldTransform_.rotation.x = std::atan2(-velosityZ.y, velosityZ.z);
}

void PlayerBullet::Update()
{
	//���W���ړ�������(�P�t���[�����̈ړ��ʂ𑫂�����)
	worldTransform_.translation += velocity_;

	worldTransform_.UpdateMatrix();

	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}
}

void PlayerBullet::Draw(ViewProjection* view)
{
	Object3d::Draw(view);
}

Vector3 PlayerBullet::GetBulletWorldPosition()
{
	// ���[���h���W������ϐ�
	Vector3 worldPos;
	// ���[���h�s��̕��s�ړ��������擾(���[���h���W)
	worldPos.x = worldTransform_.translation.x;
	worldPos.y = worldTransform_.translation.y;
	worldPos.z = worldTransform_.translation.z;

	return worldPos;
}

void PlayerBullet::OnCollision() { isDead_ = true; }
