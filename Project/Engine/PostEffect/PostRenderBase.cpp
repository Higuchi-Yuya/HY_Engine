#include "PostRenderBase.h"

ID3D12Device* PostRenderBase::sDevice_ = nullptr;

void PostRenderBase::CreateSRV(ID3D12Resource* buffer, ID3D12DescriptorHeap* descHeap)
{

	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle = descHeapSRV_->GetCPUDescriptorHandleForHeapStart();

	uint32_t descriptorSize = sDevice_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	srvCpuHandle.ptr += (SIZE_T)(descriptorSize * srvIncrementIndex_);

	// �V�F�[�_�[���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	// srv�ݒ�\����
	srvDesc.Format = buffer->GetDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = 1;

	// �n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	sDevice_->CreateShaderResourceView(buffer, &srvDesc, srvCpuHandle);

	srvIncrementIndex_++;
}

void PostRenderBase::CreateRTV(ID3D12Resource* buffer, ID3D12DescriptorHeap* descHeap)
{
}

void PostRenderBase::CreateDSV(ID3D12Resource* buffer, ID3D12DescriptorHeap* descHeap)
{
}

void PostRenderBase::DescriptorHeapInit()
{
	HRESULT result;

	// SRV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = maxSRVCount;

	// SRV�p�f�X�N���v�^�q�[�v�𐶐�
	result = sDevice_->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV_));
	assert(SUCCEEDED(result));

	// RTV�p�f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = maxRTVCount;

	// RTV�p�f�X�N���v�^�q�[�v�𐶐�
	result = sDevice_->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV_));
	assert(SUCCEEDED(result));

	// DSV�p�̃f�X�N���v�^�q�[�v�ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC DescHeapDesc{};
	DescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DescHeapDesc.NumDescriptors = 1;

	// DSV�p�f�X�N���v�^�q�[�v���쐬
	result = sDevice_->CreateDescriptorHeap(&DescHeapDesc, IID_PPV_ARGS(&descHeapDSV_));
	assert(SUCCEEDED(result));
}
