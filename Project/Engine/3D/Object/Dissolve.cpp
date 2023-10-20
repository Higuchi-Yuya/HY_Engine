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
	// 3Dオブジェクトのインスタンスを生成
	Dissolve* instance = new Dissolve();
	// 初期化
	instance->Initialize();
	// 生成したインスタンスを返す
	return instance;
}

void Dissolve::Initialize()
{
	CreateConstBuffer();
	Map();
	UpdateMatrix();

	// デフォルトテクスチャを読み込む
	dissolveTex_ = TextureManager::LoadFreeTexture(sDissolveTexBasePass_);
}

void Dissolve::UpdateMatrix()
{
	//定数バッファに転送
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
	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { TextureManager::sSrvHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(dissolveRootIndex, constBuff_->GetGPUVirtualAddress());
	if (isActiveDissolve_ == true) {
		// ディゾルブシェーダリソースビューをセット
		cmdList->SetGraphicsRootDescriptorTable(static_cast<uint32_t>(rootParameterIndex::DISSOLVETEX), dissolveTex_->GetGpuHandle());
	}
}

void Dissolve::CreateConstBuffer()
{
	assert(sDevice_);

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataDissolve) + 0xff) & ~0xff);

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

void Dissolve::Map()
{
	// 定数バッファのマッピング
	HRESULT result = constBuff_->Map(0, nullptr, (void**)&constMap_);//マッピング
	assert(SUCCEEDED(result));

	// 定数バッファの初期値を送る
	constMap_->isActiveDissolve = isActiveDissolve_;
	constMap_->dissolveColor = dissolveColor_;
	constMap_->dissolvePower = dissolvePower_;
	constMap_->dissolveTime = dissolveTime_;
	constMap_->dissolveSmoothMin = dissolveSmoothMin_;
}
