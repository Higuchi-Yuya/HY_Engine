#pragma once
#include <memory.h>
#include <list>
#include "Fruit.h"


// �v���C���[�N���X�̑O���錾
class Player;

class FruitsPop
{
public:
	FruitsPop();
	~FruitsPop();

	// ������
	void Initialize();

	// �X�V����
	void Update();

	// �`�揈��
	void Draw(ViewProjection& view);

	// �Z�b�^�[
	void SetPlayer(Player* player);

private:
	// �v���C���[
	Player* player = nullptr;

	// �t���[�c�̐������X�g
	std::list<std::unique_ptr<Fruit>> fruits;

	// �t���[�c�̃|�b�v����^�C�}�[
	int fruitPopTimer = 0;

	// �t���[�c�̃|�b�v����Ԋu
	int fruitPopInterval = 0.5 * 60;

	// �|�b�v����ʒu
	Vector3 popPos = { 0,3,1 };

	// �|�b�v������
	int fruitsNum = 0;

	// �t���[�c�̃^�C�v
	fruitType type = fruitType::apple;

	// ���f��
	Model* model = nullptr;

	Model* appleModel;
	Model* orangeModel;
	Model* bananaModel;
	Model* bombModel;

	// �X�R�A���L�^�������
	int score = 0;
};

