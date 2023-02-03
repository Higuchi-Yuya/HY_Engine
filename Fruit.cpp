#include "Fruit.h"
#include "Player.h"

Fruit::Fruit()
{
}

Fruit::~Fruit()
{
	//delete model;
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
	fruitObj->Update();

	start = Pos;
	p1 = { Pos.x+2,Pos.y - 3,Pos.z };
	p2 = { Pos.x-2,Pos.y - 6,Pos.z };
	end = { Pos.x,Pos.y - 16,Pos.z };

	points = { start,start,p1,p2,end,end };

	if ( rand() % 2 == 1) {
		IsSpline = true;
	}
	else {
		IsSpline = false;
	}

	isDeath = false;
}

void Fruit::Update()
{
	if (isDeath == false) {
		if (IsSpline == true) {
			nowCount++;
			// ����
			// �X�^�[�g�n�_		�Fstart
			// �G���h�n�_		�Fend
			// �o�ߎ���			�FelapsedTime [s]
			// �ړ������̗�(�o�ߎ���/�S�̎���) : timeRate (%)
			elapsedTime = nowCount - startCount;
			timeRate = elapsedTime / maxTime;

			if (timeRate >= 1.0f)
			{
				if (startIndex < points.size() - 3)
				{
					startIndex += 1.0f;
					timeRate -= 1.0f;
					startCount = nowCount;
				}
				else
				{
					timeRate = 1.0f;
				}
			}

			position = splinePosition(points, startIndex, timeRate);

			fruitObj->worldTransform_.position_ = position;
		}
		else if (IsSpline == false) {
			fruitObj->worldTransform_.position_.y -= 0.2f;
		}

		// �v���C���[�Ƃ̓����蔻��
		if (collision.boxCollision(player_->GetPlayerPos(), fruitObj->worldTransform_.position_,
			player_->GetBasketRadius(), fruitRadius)) {
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

Vector3 Fruit::splinePosition(const std::vector<Vector3>& points, size_t startIndex, float t)
{
	// �⊮���ׂ��_�̐�
	size_t n = points.size() - 2;

	if (startIndex > n)return points[n]; // Pn�̒l��Ԃ�
	if (startIndex < 1)return points[1]; // P1�̒l��Ԃ�

	// p0�`p3�̐���_���擾����@���@p1�`p2��⊮����
	Vector3 p0 = points[startIndex - 1];
	Vector3 p1 = points[startIndex];
	Vector3 p2 = points[startIndex + 1];
	Vector3 p3 = points[startIndex + 2];

	// Catmull - Rom�̎��ɂ����
	Vector3 position = 0.5 * (2 * p1 + (-p0 + p2) * t
		+ (2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t
		+ (-p0 + 3 * p1 - 3 * p2 + p3) * t * t * t);

	return position;
}
