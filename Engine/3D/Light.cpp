#include "Light.h"
#include <cassert>
#include <d3dx12.h>
// �ÓI�����o�ϐ��̎���
ID3D12Device* Light::device = nullptr;

void Light::StaticInititalize(ID3D12Device* device)
{
	// �ď������`�F�b�N
	assert(!Light::device);
	// nullptr�`�F�b�N
	assert(device);
	// �ÓI�����o�ϐ��̃Z�b�g
	Light::device = device;
}

Light* Light::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Light* instance = new Light();
	// ������
	instance->Initialize();
	// ���������C���X�^���X��Ԃ�
	return instance;
}

void Light::Initialize()
{
	CreateConstBuffer();
	TranferConstBuffer();
}

void Light::Update()
{
	// �l���X�V���������������萔�o�b�t�@�ɓ]������
	if (dirty) {
		TranferConstBuffer();
		dirty = false;
	}
}

void Light::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(4, constBuff->GetGPUVirtualAddress());

}

void Light::SetLightDir(const Vector3& lightdir)
{
	this->lightdir = lightdir;
	this->lightdir.normalize();
	dirty = true;
}

void Light::SetLightColor(const Vector3& lightcolor)
{
	this->lightcolor = lightcolor;
	dirty = true;
}

void Light::CreateConstBuffer()
{
	HRESULT result;

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataLight) + 0xff) & ~0xff);

	// �萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
		&heapProps, // �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));
}

void Light::TranferConstBuffer()
{
	// �o�b�t�@�}�b�v
	ConstBufferDataLight* constMap = nullptr;
	// �萔�o�b�t�@�Ƃ̃f�[�^�����N
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
	constMap->lightv = -lightdir;
	constMap->lightcolor = lightcolor;
	constBuff->Unmap(0, nullptr);
}