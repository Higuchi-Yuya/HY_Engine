#pragma once

#include "Sprite.h"

template<typename T> class Singleton;

struct Handles
{
	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle_ = D3D12_CPU_DESCRIPTOR_HANDLE(); //SRVのハンドル(CPU側)
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle_ = D3D12_GPU_DESCRIPTOR_HANDLE(); //SRVのハンドル(GPU側)
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuHandle_ = D3D12_CPU_DESCRIPTOR_HANDLE(); //RTVのハンドル(CPU側)
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuHandle_ = D3D12_CPU_DESCRIPTOR_HANDLE(); //DSVのハンドル(CPU側)
};

class PostRenderBase
{
public:
	static PostRenderBase* GetInstance()
	{
		static PostRenderBase* postRenderBase = new PostRenderBase;
		return postRenderBase;
	};

	static void Destroy()
	{
		delete GetInstance();
	}

public:// メンバ関数

	/// <summary>
	/// シェーダリソースビューの作成
	/// </summary>
	void CreateSRV(ID3D12Resource* buffer, D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle);

	/// <summary>
	/// レンダーターゲットビューの作成
	/// </summary>
	void CreateRTV(ID3D12Resource* buffer, D3D12_CPU_DESCRIPTOR_HANDLE& rtvCpuHandle);

	/// <summary>
	/// 深度ステンシルビューの作成
	/// </summary>
	void CreateDSV(ID3D12Resource* buffer, D3D12_CPU_DESCRIPTOR_HANDLE& dsvCpuHandle);

	/// <summary>
	/// デスクリプタヒープの作成
	/// </summary>
	void DescriptorHeapInit();

public:// ゲッター
	// SRVデスクのゲッター
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSrvDesc() {return descHeapSRV_;}

	// RTVデスクのゲッター
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetRtvDesc() { return descHeapRTV_; }

	// DSVデスクのゲッター
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDsvDesc() { return descHeapDSV_; }

public:
	// デバイスのセッター
	static void SetDevice(ID3D12Device* device) { sDevice_ = device; }

private:// メンバ変数

	// デバイス（借りてくる）
	static ID3D12Device* sDevice_;

	// SRVの最大個数
	const uint32_t maxSRVCount = 2056;
	// RTVの最大個数
	const uint32_t maxRTVCount = 2056;
	// DSVの最大個数
	const uint32_t maxDSVCount = 1024;

	// SRV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>descHeapSRV_;
	// RTV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>descHeapRTV_;
	// DSV用デスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>descHeapDSV_;

	// CPUハンドルのずれる量
	uint32_t srvIncrementIndex_ = 0;
	uint32_t rtvIncrementIndex_ = 0;
	uint32_t dsvIncrementIndex_ = 0;
};

