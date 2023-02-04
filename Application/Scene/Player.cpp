#include "Player.h"
#include "Input.h"
#include "SphereCollider.h"

Player* Player::Create(Model* model)
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Player* instance = new Player();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	if (!instance->Initialize()) {
		delete instance;
		assert(0);
	}

	// ���f���̃Z�b�g
	if (model) {
		instance->SetModel(model);
	}
	return instance;
}

Player::~Player()
{
	delete atariModel;
	delete atari;

}

bool Player::Initialize()
{
	if (!Object3d::Initialize()) {
		return false;
	}
	input->Initialize();
	// �R���C�_�[�̒ǉ�
	float radius = 0.6f;

	// ���a�������������畂�������W�����̒��S�ɂ���
	SetCollider(new SphereCollider(Vector3(0, radius, 0), radius));

	atariModel = new Model;
	atariModel = Model::LoadFromOBJ("sphere");

	atari = Object3d::Create();
	atari->SetModel(atariModel);
	//atari->worldTransform_.position_.y -= 8.0f;
	return true;
}

void Player::Update()
{
	
	input->Update();
	atari->Update();
	// A,D�Ő���
	if (input->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= 0.2f;
	}
	else if (input->PushKey(DIK_D)) {
		worldTransform_.rotation_.y += 0.2f;
	}

	// �ړ��x�N�g����Y������̊p�x�ŉ�]
	Vector3 move = { 0,0,0.1f };
	Matrix4 matRot;
	matRot.rotateY(worldTransform_.rotation_.y);
	move = matRot.transformNotW(move, matRot);

	// �����Ă�����Ɉړ�
	if (input->PushKey(DIK_S)) {
		worldTransform_.position_ -= move;
	}
	else if (input->PushKey(DIK_W)) {
		worldTransform_.position_ += move;
	}

	// �s��̍X�V�Ȃ�
	Object3d::Update();
}

void Player::Draw(ViewProjection* view)
{
	Object3d::Draw(view);
	atari->Draw(view);
}

void Player::OnCollision(const CollisionInfo& info)
{
	// �Փ˂�����J���[��ς���
	atari->worldTransform_.position_ = this->worldTransform_.position_;
	//worldTransform_.color_ = { 0.1f,1.0f,0.1f,1.0f };
	//worldTransform_.UpdateMatrix();
}
