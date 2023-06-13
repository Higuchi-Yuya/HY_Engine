#include "ViewProjection.h"
#include <d3dx12.h>
#include <cassert>
#include "WinApp.h"

Microsoft::WRL::ComPtr<ID3D12Device> ViewProjection::sDevice_ = nullptr;

void ViewProjection::StaticInitialize(ID3D12Device* device)
{
	assert(device);
	sDevice_ = device;
}

void ViewProjection::Initialize()
{
	CreateConstBuffer();
	Map();
	UpdateMatrix();
}

void ViewProjection::CreateConstBuffer()
{
	HRESULT result;

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataViewProjection) + 0xff) & ~0xff);

	// �萔�o�b�t�@�̐���
	result = sDevice_->CreateCommittedResource(
		&heapProps, // �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
}

void ViewProjection::Map()
{
	// �萔�o�b�t�@�Ƃ̃f�[�^�����N
	HRESULT result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));
}

void ViewProjection::MoveEyeVector(const Vector3& move)
{
	// ���_���W���ړ����A���f
	Vector3 eye_moved = eye;

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	eye = eye_moved;
}

void ViewProjection::MoveVector(const Vector3& move)
{
	// ���_�ƒ����_���W���ړ����A���f
	Vector3 eye_moved = eye;
	Vector3 target_moved = target;

	eye_moved.x += move.x;
	eye_moved.y += move.y;
	eye_moved.z += move.z;

	target_moved.x += move.x;
	target_moved.y += move.y;
	target_moved.z += move.z;

	eye = eye_moved;
	target = target_moved;
}

void ViewProjection::UpdateMatrix()
{
	// �r���[�s��̍쐬
	matView_.ViewMat(eye, target, up);
	// �ˉe�s��̍쐬
	matProjection_.ProjectionMat(fovAngleY, aspectRatio, nearZ, farZ);

	// �萔�o�b�t�@�ւ̏�������
	constMap_->view = matView_;
	constMap_->projection = matProjection_;
	constMap_->cameraPos = eye;
}

void ViewProjection::DebugCameraInitialze(Input* input)
{
	Initialize();
	assert(input);
	this->input_ = input;
	// ��ʃT�C�Y�ɑ΂��鑊�ΓI�ȃX�P�[���ɒ���
	scaleX_ = 1.0f / (float)WinApp::window_width;
	scaleY_ = 1.0f / (float)WinApp::window_height;

}

void ViewProjection::DebugCameraUpdate()
{
	bool dirty = false;
	float angleX = 0;
	float angleY = 0;

	// �}�E�X�̓��͂��擾
	Input::MouseMove mouseMove = input_->GetMouseMove();

	// �}�E�X�̍��{�^����������Ă�����J��������]������
	if (input_->PushMouseLeft()) {
		float dy = mouseMove.lX * scaleY_;
		float dx = mouseMove.lY * scaleX_;

		angleX = -dx * PI;
		angleY = -dy * PI;
		dirty = true;
	}

	// �}�E�X�̒��{�^����������Ă�����J��������s�ړ�������
	if (input_->PushMouseMiddle()) {
		float dx = mouseMove.lX / 100.0f;
		float dy = mouseMove.lY / 100.0f;

		Vector3 move = { -dx, +dy,  0 };
		move = matRot_.transform(move, matRot_);

		MoveVector(move);
		dirty = true;
	}

	// �z�C�[�����͂ŋ�����ύX
	if (mouseMove.lZ != 0) {
		distance_ -= mouseMove.lZ / 100.0f;
		distance_ = max(distance_, 1.0f);
		dirty = true;
	}

	if (dirty) {
		// �ǉ���]���̉�]�s��𐶐�
		Matrix4 matRotNew,matRotX,matRotY;
		matRotNew.identity();
		matRotX.rotateX(-angleX);
		matRotY.rotateY(-angleY);
		matRotNew = matRotX * matRotY;
		// �ݐς̉�]�s�������
		// ����]�s���ݐς��Ă����ƁA�덷�ŃX�P�[�����O��������댯�������
		// �N�H�[�^�j�I�����g�p��������]�܂���
		matRot_ = matRotNew * matRot_;

		// �����_���王�_�ւ̃x�N�g���ƁA������x�N�g��
		Vector3 vTargetEye = { 0.0f, 0.0f, -distance_};
		Vector3 vUp = { 0.0f, 1.0f, 0.0f };

		// �x�N�g������]
		vTargetEye = matRot_.transform(vTargetEye, matRot_);
		vUp = matRot_.transformNotW(vUp, matRot_);

		// �����_���炸�炵���ʒu�Ɏ��_���W������
		const Vector3& target = this->target;
		this->eye = { target.x + vTargetEye.x, target.y + vTargetEye.y,target.z + vTargetEye.z };
		this->up={ vUp.x, vUp.y, vUp.z };
	}

	UpdateMatrix();
}