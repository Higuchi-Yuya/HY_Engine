#include "Light.h"
#include <cassert>
#include <d3dx12.h>
// 静的メンバ変数の実体
ID3D12Device* Light::device = nullptr;

void Light::StaticInititalize(ID3D12Device* device)
{
	// 再初期化チェック
	assert(!Light::device);
	// nullptrチェック
	assert(device);
	// 静的メンバ変数のセット
	Light::device = device;
}

Light* Light::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	Light* instance = new Light();
	// 初期化
	instance->Initialize();
	// 生成したインスタンスを返す
	return instance;
}

void Light::Initialize()
{
	CreateConstBuffer();
	TranferConstBuffer();
}

void Light::Update()
{
	// 値が更新があった時だけ定数バッファに転送する
	if (dirty) {
		TranferConstBuffer();
		dirty = false;
	}
}

void Light::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(4, constBuff->GetGPUVirtualAddress());

}

void Light::SetLightDir(const Vector3& lightdir)
{
	this->lightdir = lightdir;
	this->lightdir.normalize();
	dirty = true;
}

void Light::SetLightColor(const Vector3& lightcolor)
{
	this->lightcolor = lightcolor;
	dirty = true;
}

void Light::CreateConstBuffer()
{
	HRESULT result;

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataLight) + 0xff) & ~0xff);

	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));
}

void Light::TranferConstBuffer()
{
	// バッファマップ
	ConstBufferDataLight* constMap = nullptr;
	// 定数バッファとのデータリンク
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
	constMap->lightv = -lightdir;
	constMap->lightcolor = lightcolor;
	constBuff->Unmap(0, nullptr);
}