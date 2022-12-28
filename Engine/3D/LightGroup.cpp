#include "LightGroup.h"
#include <cassert>
#include <d3dx12.h>
// 静的メンバ変数の実体
ID3D12Device* LightGroup::device = nullptr;

void LightGroup::StaticInititalize(ID3D12Device* device)
{
	// 再初期化チェック
	assert(!LightGroup::device);
	// nullptrチェック
	assert(device);
	// 静的メンバ変数のセット
	LightGroup::device = device;
}

LightGroup* LightGroup::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	LightGroup* instance = new LightGroup();
	// 初期化
	instance->Initialize();
	// 生成したインスタンスを返す
	return instance;
}

void LightGroup::Initialize()
{
	// ライトのデフォルトの設定
	DefaultLightSetting();
	// バッファの生成
	CreateConstBuffer();
	// バッファの転送
	TransferConstBuffer();
}

void LightGroup::Update()
{
	// 値が更新があった時だけ定数バッファに転送する
	if (dirty) {
		TransferConstBuffer();
		dirty = false;
	}
}

void LightGroup::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(4, constBuff->GetGPUVirtualAddress());

}

void LightGroup::TransferConstBuffer()
{
	// 環境光
	constMap->ambientColor = ambientColor_;

	// 平行光源
	for (int i = 0; i < kDirLightNum; i++) {
		// ライトが有効なら設定を転送
		if (dirLights_[i].IsActive()) {
			constMap->dirLights[i].active = 1;
			constMap->dirLights[i].lightv = -dirLights_[i].GetLightDir();
			constMap->dirLights[i].lightcolor = dirLights_[i].GetLightColor();
		}
		// ライトが無効ならライト色を0に
		else {
			constMap->dirLights[i].active = 0;
		}
	}
}

void LightGroup::DefaultLightSetting()
{
	dirLights_[0].SetActive(true);
	dirLights_[0].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[0].SetLightDir({ 0.0f, -1.0f, 0.0f });

	dirLights_[1].SetActive(false);
	dirLights_[1].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[1].SetLightDir({ +0.5f, +0.1f, +0.2f });

	dirLights_[2].SetActive(false);
	dirLights_[2].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[2].SetLightDir({ -0.5f, +0.1f, -0.2f });
}

void LightGroup::SetAmbientColor(const Vector3& color)
{
	ambientColor_ = color;
	dirty = true;
}

void LightGroup::SetDirLightActive(int index, bool active)
{
	assert(0 <= index && index < kDirLightNum);

	dirLights_[index].SetActive(active);
}

void LightGroup::SetDirLightDir(int index, const Vector3& lightdir)
{
	assert(0 <= index && index < kDirLightNum);

	dirLights_[index].SetLightDir(lightdir);
	dirty = true;
}

void LightGroup::SetDirLightColor(int index, const Vector3& lightcolor)
{
	assert(0 <= index && index < kDirLightNum);

	dirLights_[index].SetLightColor(lightcolor);
	dirty = true;
}

void LightGroup::CreateConstBuffer()
{
	HRESULT result;

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferData) + 0xff) & ~0xff);

	// 定数バッファの生成
	result = device->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	// 定数バッファとのデータリンク
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));
}

