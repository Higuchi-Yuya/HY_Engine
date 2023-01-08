#pragma once
#include <memory.h>
#include <vector>
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
	void Draw(ViewProjection* view);

	// �Q�b�^�[
	const int& GetScore()const { return score; }

	// �Z�b�^�[
	void SetPlayer(Player* player);

	void Finalize();

private:
	// �v���C���[
	Player* player = nullptr;

	// �t���[�c�̐������X�g
	std::vector<std::unique_ptr<Fruit>> fruits;

	// �t���[�c�̃|�b�v����^�C�}�[
	int fruitPopTimer = 0;

	// �t���[�c�̃|�b�v����Ԋu
	int fruitPopInterval = 0.5 * 60;

	// �|�b�v����ʒu
	Vector3 popPos = { 0,5,1 };

	// �|�b�v������
	int fruitsNum = 0;

	// �t���[�c�̃^�C�v
	fruitType type = fruitType::apple;

	// ���f��
	Model* appleModel;
	Model* orangeModel;
	Model* bananaModel;
	Model* bombModel;

	// �X�R�A���L�^�������
	int score = 0;
};

