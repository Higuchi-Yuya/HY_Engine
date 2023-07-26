#pragma once
#include "Sprite.h"

template<typename T> class Singleton;

struct Handles
{
	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle_ = D3D12_CPU_DESCRIPTOR_HANDLE(); //SRV�̃n���h��(CPU��)
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuHandle_ = D3D12_CPU_DESCRIPTOR_HANDLE(); //RTV�̃n���h��(CPU��)
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuHandle_ = D3D12_CPU_DESCRIPTOR_HANDLE(); //DSV�̃n���h��(CPU��)
};

class PostRenderBase
{
public:// �����o�֐�

	/// <summary>
	/// �V�F�[�_���\�[�X�r���[�̍쐬
	/// </summary>
	void CreateSRV(ID3D12Resource* buffer, ID3D12DescriptorHeap* descHeap);

	/// <summary>
	/// �����_�[�^�[�Q�b�g�r���[�̍쐬
	/// </summary>
	void CreateRTV(ID3D12Resource* buffer, ID3D12DescriptorHeap* descHeap);

	/// <summary>
	/// �[�x�X�e���V���r���[�̍쐬
	/// </summary>
	void CreateDSV(ID3D12Resource* buffer, ID3D12DescriptorHeap* descHeap);

	/// <summary>
	/// �f�X�N���v�^�q�[�v�̍쐬
	/// </summary>
	void DescriptorHeapInit();

	// �f�o�C�X�̃Z�b�^�[
	static void SetDevice(ID3D12Device* device) { sDevice_ = device; }

private:// �����o�ϐ�

	// �f�o�C�X�i�؂�Ă���j
	static ID3D12Device* sDevice_;

	// SRV�̍ő��
	const uint32_t maxSRVCount = 2056;
	// RTV�̍ő��
	const uint32_t maxRTVCount = 2056;
	// DSV�̍ő��
	const uint32_t maxDSVCount = 1;

	// SRV�p�f�X�N���v�^�q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapSRV_;
	// RTV�p�f�X�N���v�^�q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>descHeapRTV_;
	// DSV�p�f�X�N���v�^�q�[�v
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>descHeapDSV_;

	// CPU�n���h���̂�����
	uint32_t srvIncrementIndex_ = 0;
	uint32_t rtvIncrementIndex_ = 0;
	uint32_t dsvIncrementIndex_ = 0;
};

