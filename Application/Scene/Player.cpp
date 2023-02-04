#include "Player.h"
#include "Input.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"

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
	collider->SetAttribute(COLLISION_ATTR_ALLIES);

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

	// ��������
	if (!onGround) {
		// �����������x
		const float fallAcc = -0.01f;
		const float fallVYMin = -0.5f;
		// ����
		fallV.y = max(fallV.y + fallAcc, fallVYMin);
		// �ړ�
		worldTransform_.position_.x += fallV.x;
		worldTransform_.position_.y += fallV.y;
		worldTransform_.position_.z += fallV.z;
	}
	// �W�����v����
	else if (input->TriggerKey(DIK_SPACE)) {
		onGround = false;
		const float jumpVYFist = 0.2f;
		fallV = { 0, jumpVYFist, 0 };
	}

	// �s��̍X�V�Ȃ�
	Object3d::Update();

	// ���R���C�_�[���擾
	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider);
	assert(sphereCollider);

	// ���̏�[���狅�̉��[�܂ł̃��C�L���X�g�p���C������
	Ray ray;
	ray.start = sphereCollider->center;
	ray.start.y += sphereCollider->GetRadius();
	ray.dir = { 0,-1,0 };
	RaycastHit raycastHit;

	// �ڒn���
	if (onGround) {
		// �X���[�Y�ɍ������ׂ̋z������
		const float adsDistance = 0.2f;
		// �ڒn���ێ�
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance)) {
			onGround = true;
			worldTransform_.position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			// �s��̍X�V�Ȃ�
			Object3d::Update();
		}
		// �n�ʂ��Ȃ��̂ŗ���
		else {
			onGround = false;
			fallV = {0,0,0};
		}
	}
	// �������
	else if (fallV.y <= 0.0f) {
		if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f)) {
			// ���n
			onGround = true;
			worldTransform_.position_.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
			// �s��̍X�V�Ȃ�
			Object3d::Update();
		}
	}
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
