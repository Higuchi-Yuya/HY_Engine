#include "Fog.h"
#include<cassert>
#include <d3dx12.h>

Microsoft::WRL::ComPtr<ID3D12Device> Fog::device_ = nullptr;

void Fog::StaticInitialize(ID3D12Device* device)
{
	assert(device);
	device_ = device;
}

Fog* Fog::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Fog* instance = new Fog();
	// ������
	instance->Initialize();
	// ���������C���X�^���X��Ԃ�
	return instance;
}

void Fog::Initialize()
{
	CreateConstBuffer();
	Map();
	UpdateMatrix();
}

void Fog::UpdateMatrix()
{
	//�萔�o�b�t�@�ɓ]��
	constMap->isActiveFog = this->isActiveFog;
	constMap->fogColor = this->fogColor;
	constMap->nearFog = this->nearFog;
	constMap->farFog = this->farFog;
}

void Fog::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(5, constBuff->GetGPUVirtualAddress());
}

void Fog::CreateConstBuffer()
{
	assert(device_);

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataFog) + 0xff) & ~0xff);

	HRESULT result;

	// �萔�o�b�t�@�̐���
	result = device_->CreateCommittedResource(
		&heapProps, // �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	assert(SUCCEEDED(result));
}

void Fog::Map()
{
	//�萔�o�b�t�@�̃}�b�s���O
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);//�}�b�s���O
	assert(SUCCEEDED(result));
}
