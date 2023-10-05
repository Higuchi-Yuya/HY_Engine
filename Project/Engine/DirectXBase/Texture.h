#pragma once

#include"DirectXCommon.h"
#include<wrl.h>
#include<array>
#include<string>
#include <d3dx12.h>
#include <DirectXTex.h>
#include "Vector2.h"

class Texture
{
private:
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_ = D3D12_CPU_DESCRIPTOR_HANDLE(); //SRVのハンドル(CPU側)
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_ = D3D12_GPU_DESCRIPTOR_HANDLE(); //SRVのハンドル(GPU側)

public:
	Microsoft::WRL::ComPtr<ID3D12Resource> buffer; //テクスチャのリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
	Vector2 size = {0.0f,0.0f};
	bool isMaterial = false;

public:
	Texture() {};

public: // セッター
	inline void SetCpuHandle(const D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle) { cpuHandle_ = cpuHandle; }
	inline void SetGpuHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle) { gpuHandle_ = gpuHandle; }

public: // ゲッター
	inline D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() { return gpuHandle_; }
};

