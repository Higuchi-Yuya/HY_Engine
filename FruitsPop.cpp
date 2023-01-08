#include "FruitsPop.h"
#include "Player.h"

FruitsPop::FruitsPop()
{
	appleModel = Model::LoadFromOBJ("apple",true);
	orangeModel = Model::LoadFromOBJ("orange",true);
	bananaModel = Model::LoadFromOBJ("banana");
	bombModel = Model::LoadFromOBJ("bomb",true);

}

FruitsPop::~FruitsPop()
{
	//fruits.clear();
	delete appleModel;
	delete orangeModel;
	delete bananaModel;
	delete bombModel;

}

void FruitsPop::Initialize()
{
	fruitPopTimer = 0;
	fruitPopInterval = 0.5 * 60;
	score = 0;
	fruits.clear();
}

void FruitsPop::Update()
{
	
	fruitPopTimer++;

	// �t���[�c�̏��Ńt���O�����Ă����烊�X�g�������
	//fruits.remove_if([](std::unique_ptr<Fruit>& fruit) { return fruit->IsDeath(); });

	// �t���[�c�̃|�b�v�^�C���ɂȂ�����|�b�v
	if (fruitPopTimer >= fruitPopInterval) {
		// �����_���Ƀ|�W�V�����𒊑I
		// �͈͂́i-10�`10�j
		popPos.x = rand() % 21 - 10;

		// �����_���Ɏ�ނ𒊑I (0�`3)
		fruitsNum = rand() % 4;

		// �t���[�c�̔ԍ��Ń^�C�v�̎w��
		switch (fruitsNum)
		{
		case 0:// ���
			type = fruitType::apple;
			break;

		case 1:// �I�����W
			type = fruitType::orange;
			break;

		case 2:// �o�i�i
			type = fruitType::banana;
			break;

		case 3:// ���e
			type = fruitType::bomb;
			break;

		default:
			break;
		}

		// �t���[�c�̃^�C�v�Ń��f���̎w��
		switch (type)
		{
		case fruitType::apple:
		{
			// �t���[�c�̐����A������
			std::unique_ptr<Fruit> newFruit = std::make_unique<Fruit>();

			newFruit->Initialize(popPos, appleModel);
			newFruit->typeNum = 0;
			// ���X�g�ɓo�^
			fruits.push_back(std::move(newFruit));
		}
		break;
		case fruitType::orange:
		{
			// �t���[�c�̐����A������
			std::unique_ptr<Fruit> newFruit = std::make_unique<Fruit>();

			newFruit->Initialize(popPos, orangeModel);
			newFruit->typeNum = 1;
			// ���X�g�ɓo�^
			fruits.push_back(std::move(newFruit));
		}
		break;
		case fruitType::banana:
		{
			// �t���[�c�̐����A������
			std::unique_ptr<Fruit> newFruit = std::make_unique<Fruit>();

			newFruit->Initialize(popPos, bananaModel);
			newFruit->typeNum = 2;
			// ���X�g�ɓo�^
			fruits.push_back(std::move(newFruit));
		}
		break;
		case fruitType::bomb:
		{
			// �t���[�c�̐����A������
			std::unique_ptr<Fruit> newFruit = std::make_unique<Fruit>();

			newFruit->Initialize(popPos, bombModel);
			newFruit->typeNum = 3;
			// ���X�g�ɓo�^
			fruits.push_back(std::move(newFruit));
		}
		break;
		default:
			break;
		}

		// �|�b�v�^�C�}�[�̃��Z�b�g
		fruitPopTimer = 0;
	}

	// �t���[�c�̃��X�g�̍X�V�����Ăяo��
	if (fruits.size() >= 1) {
		for (std::unique_ptr<Fruit>& fruit : fruits) {
			fruit->SetPlayer(player);
			fruit->Update();

			if (fruit->IsHit() == true && fruit->IsPlus() == false) {
				switch (fruit->typeNum)
				{
				case 0:// ��񂲂̓_��
					score += 5;
					fruit->SetIsPlus(true);
					break;
				case 1:// �I�����W�̓_��
					score += 5;
					fruit->SetIsPlus(true);
					break;
				case 2:// �o�i�i�̓_��
					score += 10;
					fruit->SetIsPlus(true);
					break;
				case 3:// ���e�̓_��
					score -= 20;
					fruit->SetIsPlus(true);
				default:
					break;
				}
			}
		}
	}


	// �X�R�A���O�ȉ��ɂȂ�����O�ɂ���
	if (score <= 0) {
		score = 0;
	}
}

void FruitsPop::Draw(ViewProjection* view)
{
	// �t���[�c�̏��Ńt���O�����Ă����烊�X�g�������
	//fruits.remove_if([](std::unique_ptr<Fruit>& fruit) { return fruit->IsDeath(); });
	if (fruits.size() >= 1) {
		for (std::unique_ptr<Fruit>& fruit : fruits) {
			fruit->Draw(view);
		}
	}

}

void FruitsPop::SetPlayer(Player* player)
{
	this->player = player;
}

void FruitsPop::Finalize()
{
	delete appleModel;
	delete orangeModel;
	delete bananaModel;
	delete bombModel;
}
