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
	// 3Dオブジェクトのインスタンスを生成
	Fog* instance = new Fog();
	// 初期化
	instance->Initialize();
	// 生成したインスタンスを返す
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
	//定数バッファに転送
	constMap_->isActiveFog = isActiveFog;
	constMap_->fogColor = fogColor;
	constMap_->nearFog = nearFog;
	constMap_->farFog = farFog;
}

void Fog::Draw(ID3D12GraphicsCommandList* cmdList, uint32_t fogRootIndex)
{
	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(fogRootIndex, constBuff_->GetGPUVirtualAddress());
}

void Fog::CreateConstBuffer()
{
	assert(sDevice_);

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataFog) + 0xff) & ~0xff);

	HRESULT result;

	// 定数バッファの生成
	result = sDevice_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));

	assert(SUCCEEDED(result));
}

void Fog::Map()
{
	//定数バッファのマッピング
	HRESULT result = constBuff_->Map(0, nullptr, (void**)&constMap_);//マッピング
	assert(SUCCEEDED(result));
}
