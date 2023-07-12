#include "Player.h"
#include "Input.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "CollisionAttribute.h"
#include "JoyPadInput.h"

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


}

bool Player::Initialize()
{
	if (!Object3d::Initialize()) {
		return false;
	}
	//input->Initialize();
	// �R���C�_�[�̒ǉ�
	float radius = 0.6f;
	//worldTransform_.position_.y += 100;
	// ���a�������������畂�������W�����̒��S�ɂ���
	//SetCollider(new SphereCollider(Vector3(0, radius, 0), radius));
	//collider_->SetAttribute(COLLISION_ATTR_ALLIES);

	return true;
}

void Player::Update()
{

	MoveUpdate();

	//// ��������
	//if (!onGround) {
	//	// �����������x
	//	const float fallAcc = -0.01f;
	//	const float fallVYMin = -0.5f;
	//	// ����
	//	fallV.y = max(fallV.y + fallAcc, fallVYMin);
	//	// �ړ�
	//	worldTransform_.translation.x += fallV.x;
	//	worldTransform_.translation.y += fallV.y;
	//	worldTransform_.translation.z += fallV.z;
	//}
	//// �W�����v����
	//else if (input->TriggerKey(DIK_SPACE)) {
	//	onGround = false;
	//	const float jumpVYFist = 0.2f;
	//	fallV = { 0, jumpVYFist, 0 };
	//}

	//// ���[���h�s��X�V
	//UpdateWorldMatrix();
	//collider_->Update(worldTransform_.matWorld_);

	//// ���R���C�_�[���擾
	//SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider_);
	//assert(sphereCollider);

	//// �N�G���[�R�[���o�b�N�N���X
	//class PlayerQueryCallback : public QueryCallback
	//{
	//public:
	//	PlayerQueryCallback(Sphere* sphere) : sphere(sphere) {};

	//	// �Փˎ��R�[���o�b�N�֐�
	//	bool OnQueryHit(const QueryHit& info) {

	//		const Vector3 up = { 0,1,0 };
	//		Vector3 info_ = info.reject;
	//		Vector3 rejectDir = info_.normalize();
	//		float cos = Vector3::dot(rejectDir, up);

	//		// �n�ʔ��肵�����l
	//		const float threshold = cosf(Vector3::Deg2Rad(30.0f));

	//		if (-threshold < cos && cos < threshold) {
	//			sphere->center += info.reject;
	//			move += info.reject;
	//		}

	//		return true;
	//	}

	//	Sphere* sphere = nullptr;
	//	Vector3 move = {};
	//};

	//PlayerQueryCallback callback(sphereCollider);

	//// ���ƒn�`�̌�����S����
	//CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &callback, COLLISION_ATTR_LANDSHAPE);

	//// �����ɂ��r�˕�������
	//worldTransform_.translation += callback.move;


	//// ���[���h�s��X�V
	//UpdateWorldMatrix();
	//collider_->Update(worldTransform_.matWorld_);

	//// ���̏�[���狅�̉��[�܂ł̃��C�L���X�g�p���C������
	//Ray ray;
	//ray.start = sphereCollider->center;
	//ray.start.y += sphereCollider->GetRadius();
	//ray.dir = { 0,-1,0 };
	//RaycastHit raycastHit;

	//// �ڒn���
	//if (onGround) {
	//	// �X���[�Y�ɍ������ׂ̋z������
	//	const float adsDistance = 0.2f;
	//	// �ڒn���ێ�
	//	if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f + adsDistance)) {
	//		onGround = true;
	//		worldTransform_.translation.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);
	//		// �s��̍X�V�Ȃ�
	//		Object3d::Update();
	//	}
	//	// �n�ʂ��Ȃ��̂ŗ���
	//	else {
	//		onGround = false;
	//		fallV = {0,0,0};
	//	}
	//}
	//// �������
	//else if (fallV.y <= 0.0f) {
	//	if (CollisionManager::GetInstance()->Raycast(ray, COLLISION_ATTR_LANDSHAPE, &raycastHit, sphereCollider->GetRadius() * 2.0f)) {
	//		// ���n
	//		onGround = true;
	//		worldTransform_.translation.y -= (raycastHit.distance - sphereCollider->GetRadius() * 2.0f);

	//	}
	//}

	// �s��̍X�V�Ȃ�
	Object3d::Update();
}

void Player::Draw(ViewProjection* view)
{
	Object3d::Draw(view);
	//atari->Draw(view);
}

void Player::OnCollision(const CollisionInfo& info)
{
	// �Փ˂�����J���[��ς���
	//atari->worldTransform_.translation = this->worldTransform_.translation;
	//worldTransform_.color_ = { 0.1f,1.0f,0.1f,1.0f };
	//worldTransform_.UpdateMatrix();
}

const Vector3 Player::GetWorldPosition() const
{
	//���[���h���W������ϐ�
	Vector3 worldPos;
	//���[���h�s��ړ��������擾(���[���h���W)
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::MoveUpdate()
{
	Matrix4 mathMat;

	// �ړ��x�N�g����Y������̊p�x�ŉ�]

	Vector3 vectorX = { 0.01f,0,0 };
	vectorX = MathUtil::MatVector(worldTransform_.matWorld_,vectorX);
	vectorX.normalize();
	Vector3 vectorZ = { 0,0,-0.01f };
	vectorZ = MathUtil::MatVector(worldTransform_.matWorld_, vectorZ);
	vectorZ.normalize();

	vectorX *= moveValue;
	vectorZ *= moveValue;

	Vector3 move = { 0,0,0 };
	Vector3 rot = { 0,0,0 };

	Vector2 joyStickInfoL = { 0,0 };
	Vector2 joyStickInfoR = { 0,0 };
	//Matrix4 matRot;
	//matRot.rotateY(worldTransform_.rotation.y);
	//move = matRot.transformNotW(move, matRot);

	// �����Ă�����Ɉړ�

	// L�X�e�B�b�N�ňړ�
	if (JoypadInput::GetStick(PadCode::LeftStick).x > deadZone ||
		JoypadInput::GetStick(PadCode::LeftStick).x < -deadZone ||
		JoypadInput::GetStick(PadCode::LeftStick).y > deadZone ||
		JoypadInput::GetStick(PadCode::LeftStick).y < -deadZone) {

		move.x += JoypadInput::GetStick(PadCode::LeftStick).x / 1000 * vectorX.x;
		move.z += JoypadInput::GetStick(PadCode::LeftStick).x / 1000 * vectorX.z;
		move.x += JoypadInput::GetStick(PadCode::LeftStick).y / 1000 * vectorZ.x;
		move.z += JoypadInput::GetStick(PadCode::LeftStick).y / 1000 * vectorZ.z;

		joyStickInfoL.x = JoypadInput::GetStick(PadCode::LeftStick).x;
		joyStickInfoL.y = JoypadInput::GetStick(PadCode::LeftStick).y;

	}

	// R�X�e�B�b�N�ŃJ������]
	if (JoypadInput::GetStick(PadCode::RightStick).x > deadZone ||
		JoypadInput::GetStick(PadCode::RightStick).x < -deadZone) {
		rot.y = JoypadInput::GetStick(PadCode::RightStick).x / 1000 * 2;

		joyStickInfoR.y = JoypadInput::GetStick(PadCode::RightStick).x / 1000;
	}

	worldTransform_.rotation.y += MathUtil::DegreeToRadian(rot.y);
	worldTransform_.translation += move;
	

	ImGui::Begin("joyPadInfo");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));

	ImGui::InputFloat2("joySrickL", &joyStickInfoL.x, "%.2f");
	ImGui::InputFloat2("joySrickR", &joyStickInfoR.x, "%.2f");
	ImGui::InputFloat3("playerPos", &worldTransform_.translation.x, "%.2f");

	ImGui::End();


	// ���[���h�s��X�V
	UpdateWorldMatrix();
}
