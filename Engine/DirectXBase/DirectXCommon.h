#pragma once
#include <d3d12.h>
#include<dxgi1_6.h>
#include <wrl.h>
#include <vector>
#include <chrono>
#include "WinApp.h"

class DirectXCommon
{
public:// �����o�֐�
	// ������
	void Initalize(WinApp* winApp);

	// �`��O����
	void PreDraw();

	// �`��㏈��
	void PostDraw();

	/// <summary>
	/// �[�x�o�b�t�@�̃N���A
	/// </summary>
	void ClearDepthBuffer();

	// �f�o�C�X�擾
	ID3D12Device* GetDevice() const { return device_.Get();}

	// �R�}���h���X�g�擾
	ID3D12GraphicsCommandList* GetCommandList()const { return commandList_.Get(); }
	
	// �o�b�N�o�b�t�@�̐����擾
	size_t GetBackBufferCount() const { return backBuffers_.size(); }

	// �R�}���h�L���A���擾
	ID3D12CommandQueue* GetCommandQueue()const { return commandQueue_.Get(); }

	// �t�F���X�̗ʂ𑝂₷
	inline void PreIncrimentFenceValue() { ++fenceVal_; }

	// �t�F���X�擾
	inline ID3D12Fence* GetFence() const { return fence_.Get(); }
	inline uint64_t GetFenceValue() { return fenceVal_; }

	// �R�}���h�A���P�[�^���擾
	inline ID3D12CommandAllocator* GetCommandAllocator() const { return commandAllocator_.Get(); }

private:
	// �f�o�C�X�̏�����
	void InitalizeDevice();

	// �R�}���h�֘A�̏�����
	void InitialzeCommand();

	// �X���b�v�`�F�[���̏�����
	void InitializeSwapchain(WinApp* winApp);

	// �����_�[�^�[�Q�b�g�r���[�̏�����
	void InitializeRenderTargetView();

	// �[�x�o�b�t�@�̏�����
	void InitializeDepthBuffer();

	// �t�F���X�̏�����
	void InitializeFence();

	// FPS�Œ菉����
	void InitializeFixFPS();

	// FPS�Œ�X�V
	void UpdateFixFPS();

private:
	// WindowsAPI
	WinApp* winApp_ = nullptr;
	// DirectX12�f�o�C�X
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	// DXGI�t�@�N�g��
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;

	// �R�}���h�֘A
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;

	// �X���b�v�`�F�[���֘A
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};

	// �����_�[�^�[�Q�b�g�r���[�֘A
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_ = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc_{};

	// �o�b�N�o�b�t�@
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> backBuffers_;

	// �[�x�o�b�t�@�֘A
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuff_;

	// ���\�[�X�o���A
	D3D12_RESOURCE_BARRIER barrierDesc_{};

	// �t�F���X
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
	uint64_t fenceVal_ = 0;

	// ���ʊm�F
	HRESULT result;

	// �L�^����(FPS�Œ�p)
	std::chrono::steady_clock::time_point reference_;
};

