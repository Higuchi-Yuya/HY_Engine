#pragma once
#include "Object3d.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Quaternion.h"
#include "Light.h"
#include "Input.h"
class Player
{
public:
	Player();
	~Player();

	// ������
	void Initialize();
	// �X�V����
	void Update();
	// �`�揈��
	void Draw(ViewProjection *view);

	// �Q�b�^�[
	// �|�W�V����
	const Vector3& GetPlayerPos() const {
		Vector3 pos = playerObj->worldTransform_.position_;
		pos.y += 1.5f;
		return  pos;}

	// �����̔��a
	const Vector3& GetBasketRadius() const { return playerBasketRadius; }

private:
	// �C���v�b�g
	Input* input = nullptr;

	// ���f��
	Model* model = nullptr;

	// �I�u�W�F�N�g
	Object3d* playerObj = nullptr;

	Object3d* objCollision;

	// �v���C���[�̂����̑傫��
	Vector3 playerBasketRadius = { 0.8f,0.2f,1.0f };
};

