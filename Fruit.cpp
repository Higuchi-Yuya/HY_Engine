#include "Fruit.h"
#include "Player.h"

Fruit::Fruit()
{
}

Fruit::~Fruit()
{
	delete model;
	delete fruitObj;
}


void Fruit::Initialize(Vector3 Pos, Model* model)
{
	// NULL�|�C���^�`�F�b�N
	assert(model);

	this->model = model;
	
	// �I�u�W�F�N�g�̏�����
	fruitObj = Object3d::Create();

	// ���f���̃Z�b�g
	fruitObj->SetModel(model);

	// �����̃|�W�V�����Z�b�g
	fruitObj->worldTransform_.position_ = Pos;

	isDeath = false;
}

void Fruit::Update(Player* player)
{
	if (isDeath == false) {
		// ����
		fruitObj->worldTransform_.position_.y -= 0.2f;

		// �v���C���[�Ƃ̓����蔻��
		if (collision.boxCollision(player->GetPlayerPos(), fruitObj->worldTransform_.position_,
			player->GetBasketRadius(), fruitRadius)) {
			// �������Ă�����t���[�c������
			isDeath = true;
			isHit = true;
		}

		// ��ʊO����
		if (fruitObj->worldTransform_.position_.y <= -9.0f) {
			// ��ʂ̊O�ɂ����������
			isDeath = true;
		}

		// �s��̍X�V�Ɠ]��
		fruitObj->Update();
	}
}

void Fruit::Draw(ViewProjection* view)
{
	if (isDeath == false) {
		fruitObj->Draw(view);
	}
	
}
