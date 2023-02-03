#pragma once
#include <cassert>
#include "Object3d.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Collision.h"
class Player;

// �t���[�c�̃^�C�v
enum class fruitType {
	apple,
	orange,
	banana,
	bomb
};
class Fruit
{

public:
	Fruit();
	~Fruit();

	// ������
	void Initialize(Vector3 Pos,Model* model);
	// �X�V����
	void Update();
	// �`�揈��
	void Draw(ViewProjection* view);

	void SetPlayer(Player* player) { player_ = player; }

	// ���S�t���O��n��
	bool IsDeath() const { return isDeath; }

	// �q�b�g�t���O��n��
	bool IsHit()const { return isHit; }

	// ���Z�t���O��n��
	bool IsPlus() const { return isPlus; }

	void SetIsPlus(bool isPlus) { this->isPlus = isPlus; }

	// ��ނ̔ԍ�
	int typeNum = 0;

	Vector3 splinePosition(const std::vector<Vector3>& points, size_t startIndex, float t);

private:
	// ���f��
	Model* model = nullptr;

	// �I�u�W�F�N�g
	Object3d* fruitObj = nullptr;

	// ���Ńt���O
	bool isDeath = false;

	// �Փ˃t���O
	bool isHit = false;

	// �_�������Z���ꂩ�̃t���O
	bool isPlus = false;

	// �t���[�c�̈�̔��a
	Vector3 fruitRadius = { 1.0f,0.8f,1.0f };

	// �����蔻��
	Collision collision;

	Player* player_ = nullptr;

	bool IsSpline = false;
#pragma region �X�v���C���Ȑ�
	// �X�^�[�g�̃|�W�V����
	Vector3 start;
	// �r���̃|�W�V�����P
	Vector3 p1;
	// �r���̃|�W�V�����Q
	Vector3 p2;
	// �I���̃|�W�V����
	Vector3 end;

	std::vector<Vector3> points;
	size_t startIndex = 1;
	// �ړ��̈ʒu
	Vector3 position;

	// �X�^�[�g���鎞��
	float startCount = 0;
	// ���̎���
	float nowCount = 0;
	// �o�ߎ���
	float elapsedTime = 0;
	// �S�̂̎���
	float maxTime = 1.2f * 60;
	// �ړ������̗�(�o�ߎ��� / �S�̎���) : timeRate(%)
	float timeRate;
#pragma endregion

};

