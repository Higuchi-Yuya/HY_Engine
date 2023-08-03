#include "Dissolve.h"
#include "Object3d.h"
#include<cassert>

ID3D12Device* Dissolve::sDevice_ = nullptr;

const std::string Dissolve::sDissolveTexBasePass_ = "Resources/3D_Resources/DefaultMaterial/DissolveMap.png";

Dissolve::~Dissolve()
{
	delete dissolveTex_;
}

void Dissolve::StaticInitialize(ID3D12Device* device)
{
	assert(device);
	sDevice_ = device;
}

Dissolve* Dissolve::Create()
{
	// 3D�I�u�W�F�N�g�̃C���X�^���X�𐶐�
	Dissolve* instance = new Dissolve();
	// ������
	instance->Initialize();
	// ���������C���X�^���X��Ԃ�
	return instance;
}

void Dissolve::Initialize()
{
	CreateConstBuffer();
	Map();
	UpdateMatrix();

	// �f�t�H���g�e�N�X�`����ǂݍ���
	dissolveTex_ = TextureManager::LoadFreeTexture(sDissolveTexBasePass_);
}

void Dissolve::UpdateMatrix()
{
	//�萔�o�b�t�@�ɓ]��
	if (isActiveDissolve_ == true) {
		constMap_->isActiveDissolve = isActiveDissolve_;
		constMap_->dissolveColor = dissolveColor_;
		constMap_->dissolvePower = dissolvePower_;
		constMap_->dissolveTime = dissolveTime_;
		constMap_->dissolveSmoothMin = dissolveSmoothMin_;
	}

}

void Dissolve::Draw(ID3D12GraphicsCommandList* cmdList, uint32_t dissolveRootIndex)
{
	// �萔�o�b�t�@�r���[���Z�b�g
	cmdList->SetGraphicsRootConstantBufferView(dissolveRootIndex, constBuff_->GetGPUVirtualAddress());
	if (isActiveDissolve_ == true) {
		// �f�B�]���u�V�F�[�_���\�[�X�r���[���Z�b�g
		cmdList->SetGraphicsRootDescriptorTable(static_cast<uint32_t>(rootParameterIndex::DISSOLVETEX), dissolveTex_->GetGpuHandle());
	}
}

void Dissolve::CreateConstBuffer()
{
	assert(sDevice_);

	// �q�[�v�v���p�e�B
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// ���\�[�X�ݒ�
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataDissolve) + 0xff) & ~0xff);

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

void Dissolve::Map()
{
	// �萔�o�b�t�@�̃}�b�s���O
	HRESULT result = constBuff_->Map(0, nullptr, (void**)&constMap_);//�}�b�s���O
	assert(SUCCEEDED(result));

	// �萔�o�b�t�@�̏����l�𑗂�
	constMap_->isActiveDissolve = isActiveDissolve_;
	constMap_->dissolveColor = dissolveColor_;
	constMap_->dissolvePower = dissolvePower_;
	constMap_->dissolveTime = dissolveTime_;
	constMap_->dissolveSmoothMin = dissolveSmoothMin_;
}
