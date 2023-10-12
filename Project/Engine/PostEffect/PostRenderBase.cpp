#include "PostRenderBase.h"


ID3D12Device* PostRenderBase::sDevice_ = nullptr;

void PostRenderBase::CreateSRV(ID3D12Resource* buffer,D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE SrvCpuHandle = descHeapSRV_->GetCPUDescriptorHandleForHeapStart();

	uint32_t descriptorSize = sDevice_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	SrvCpuHandle.ptr += (SIZE_T)(descriptorSize * srvIncrementIndex_);

	// ハンドルの情報を代入
	srvCpuHandle = SrvCpuHandle;

	// シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	// srv設定構造体
	srvDesc.Format = buffer->GetDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;

	// ハンドルの指す位置にシェーダーリソースビュー作成
	sDevice_->CreateShaderResourceView(buffer, &srvDesc, srvCpuHandle);

	srvIncrementIndex_++;
}

void PostRenderBase::CreateRTV(ID3D12Resource* buffer, D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE RtvCpuHandle = descHeapRTV_->GetCPUDescriptorHandleForHeapStart();

	uint32_t descriptorSize = sDevice_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	RtvCpuHandle.ptr += (SIZE_T)(descriptorSize * rtvIncrementIndex_);

	// ハンドルの情報を代入
	rtvCpuHandle = RtvCpuHandle;

	// レンダーターゲットビュー設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};	// rtv設定構造体

	// シェーダの計算結果をSRGBに変換して書き込む
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// ハンドルの指す位置にレンダーターゲットビュー作成
	sDevice_->CreateRenderTargetView(buffer, &rtvDesc, rtvCpuHandle);

	rtvIncrementIndex_++;
}

void PostRenderBase::CreateDSV(ID3D12Resource* buffer, D3D12_CPU_DESCRIPTOR_HANDLE dsvCpuHandle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE DsvCpuHandle = descHeapDSV_->GetCPUDescriptorHandleForHeapStart();

	uint32_t descriptorSize = sDevice_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	DsvCpuHandle.ptr += (SIZE_T)(descriptorSize * dsvIncrementIndex_);

	// ハンドルの情報を代入
	dsvCpuHandle = DsvCpuHandle;

	// デスクリプタヒープにDSV作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;

	// ハンドルの指す位置に深度ステンシルビュー作成
	sDevice_->CreateDepthStencilView(buffer,&dsvDesc,dsvCpuHandle);

	dsvIncrementIndex_++;
}

void PostRenderBase::DescriptorHeapInit()
{
	HRESULT result;

	// SRV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC srvDescHeapDesc = {};
	srvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvDescHeapDesc.NumDescriptors = maxSRVCount;

	// SRV用デスクリプタヒープを生成
	result = sDevice_->CreateDescriptorHeap(&srvDescHeapDesc, IID_PPV_ARGS(&descHeapSRV_));
	assert(SUCCEEDED(result));

	// RTV用デスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescHeapDesc{};
	rtvDescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescHeapDesc.NumDescriptors = maxRTVCount;

	// RTV用デスクリプタヒープを生成
	result = sDevice_->CreateDescriptorHeap(&rtvDescHeapDesc, IID_PPV_ARGS(&descHeapRTV_));
	assert(SUCCEEDED(result));

	// DSV用のデスクリプタヒープ設定
	D3D12_DESCRIPTOR_HEAP_DESC DescHeapDesc{};
	DescHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DescHeapDesc.NumDescriptors = 1;

	// DSV用デスクリプタヒープを作成
	result = sDevice_->CreateDescriptorHeap(&DescHeapDesc, IID_PPV_ARGS(&descHeapDSV_));
	assert(SUCCEEDED(result));
}
