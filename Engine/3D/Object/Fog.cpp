#include "Fog.h"
#include<cassert>
#include <d3dx12.h>

ID3D12Device* Fog::sDevice_ = nullptr;

void Fog::StaticInitialize(ID3D12Device* device)
{
	assert(device);
	sDevice_ = device;
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
	constMap_->isActiveFog = isActiveFog;
	constMap_->fogColor = fogColor;
	constMap_->nearFog = nearFog;
	constMap_->farFog = farFog;
}

void Fog::Draw(ID3D12GraphicsCommandList* cmdList, uint32_t fogRootIndex)
{
	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(fogRootIndex, constBuff_->GetGPUVirtualAddress());
}

void Fog::CreateConstBuffer()
{
	assert(sDevice_);

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataFog) + 0xff) & ~0xff);

	HRESULT result;

	// �萔�o�b�t�@�̐���
	result = sDevice_->CreateCommittedResource(
		&heapProps, // �A�b�v���[�h�\
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));

	assert(SUCCEEDED(result));
}

void Fog::Map()
{
	//�萔�o�b�t�@�̃}�b�s���O
	HRESULT result = constBuff_->Map(0, nullptr, (void**)&constMap_);//�}�b�s���O
	assert(SUCCEEDED(result));
}
