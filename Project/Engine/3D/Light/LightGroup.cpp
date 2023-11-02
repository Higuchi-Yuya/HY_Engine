#include "LightGroup.h"
#include <cassert>
#include <d3dx12.h>

// 静的メンバ変数の実体
ID3D12Device* LightGroup::sDevice = nullptr;

void LightGroup::StaticInititalize(ID3D12Device* device)
{
	// 再初期化チェック
	assert(!LightGroup::sDevice);
	// nullptrチェック
	assert(device);
	// 静的メンバ変数のセット
	LightGroup::sDevice = device;
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
	if (dirty_) {
		TransferConstBuffer();
		dirty_ = false;
	}
}

void LightGroup::Draw(ID3D12GraphicsCommandList* cmdList,uint32_t rootParameterIndex)
{
	// 定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(rootParameterIndex, constBuff_->GetGPUVirtualAddress());

}

void LightGroup::TransferConstBuffer()
{
	// 環境光
	constMap_->ambientColor = ambientColor_;

	// ディフューズカラー
	constMap_->diffuseColor = diffuseColor_;

	// スペキュラーカラー
	constMap_->specularColor = specularColor_;

	// 平行光源
	for (int i = 0; i < sDirLightNum; i++) {
		// ライトが有効なら設定を転送
		if (dirLights_[i].IsActive()) {
			constMap_->dirLights[i].active = 1;
			constMap_->dirLights[i].lightv = -dirLights_[i].GetLightDir();
			constMap_->dirLights[i].lightcolor = dirLights_[i].GetLightColor();
		}
		// ライトが無効ならライト色を0に
		else {
			constMap_->dirLights[i].active = 0;
		}
	}

	// 点光源
	for (int i = 0; i < sPointLightNum; i++) {
		// ライトが有効なら設定を転送
		if (pointLights_[i].IsActive()) {
			constMap_->pointLights[i].active = 1;
			constMap_->pointLights[i].lightpos = pointLights_[i].GetLightPos();
			constMap_->pointLights[i].lightcolor = pointLights_[i].GetLightColor();
			constMap_->pointLights[i].lightatten = pointLights_[i].GetLightAtten();
			constMap_->pointLights[i].lihgtIndensity = pointLights_[i].GetIndensity();
			constMap_->pointLights[i].lightRadius = pointLights_[i].GetRadius();
			constMap_->pointLights[i].lightDecay = pointLights_[i].GetDecay();
		}
		// ライトが無効ならライト色を0に
		else {
			constMap_->pointLights[i].active = 0;
		}
	}

	// スポットライト
	for (int i = 0; i < sSpotLightNum; i++) {
		// ライトが有効なら設定を転送
		if (spotLights_[i].IsActive()) {
			constMap_->spotLights[i].active = 1;
			constMap_->spotLights[i].lightv = -spotLights_[i].GetLightDir();
			constMap_->spotLights[i].lightpos = spotLights_[i].GetLightPos();
			constMap_->spotLights[i].lightcolor = spotLights_[i].GetLightColor();
			constMap_->spotLights[i].lightatten = spotLights_[i].GetLightAtten();
			constMap_->spotLights[i].lightfactoranglecos = spotLights_[i].GetLightFactorAngleCos();
		}
		// ライトが無効ならライト色を0に
		else {
			constMap_->spotLights[i].active = 0;
		}
	}

	// 丸影
	for (int i = 0; i < sCircleShadowNum; i++) {
		// 有効なら設定を転送
		if (circleShadows_[i].IsActive()) {
			constMap_->circleShadows[i].active = 1;
			constMap_->circleShadows[i].dir = -circleShadows_[i].GetDir();
			constMap_->circleShadows[i].casterPos = circleShadows_[i].GetCasterPos();
			constMap_->circleShadows[i].distanceCasterLight = circleShadows_[i].GetDistanceCasterLight();
			constMap_->circleShadows[i].atten = circleShadows_[i].GetAtten();
			constMap_->circleShadows[i].factorAngleCos = circleShadows_[i].GetFactorAngleCos();
		}
		// 無効なら色を0に
		else {
			constMap_->circleShadows[i].active = 0;
		}
	}
}

void LightGroup::DefaultLightSetting()
{
	dirLights_[0].SetActive(true);
	dirLights_[0].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[0].SetLightDir({ 0.0f, -1.0f, 0.0f });

	dirLights_[1].SetActive(true);
	dirLights_[1].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[1].SetLightDir({ +0.5f, +0.1f, +0.2f });

	dirLights_[2].SetActive(true);
	dirLights_[2].SetLightColor({ 1.0f, 1.0f, 1.0f });
	dirLights_[2].SetLightDir({ -0.5f, +0.1f, -0.2f });
}

void LightGroup::SetAmbientColor(const Vector3& color)
{
	ambientColor_ = color;
	dirty_ = true;
}

void LightGroup::SetDiffuseColor(const Vector3& color)
{
	diffuseColor_ = color;
	dirty_ = true;
}

void LightGroup::SetSpecularColor(const Vector3& color)
{
	specularColor_ = color;
	dirty_ = true;
}

void LightGroup::SetDirLightActive(int index, bool active)
{
	assert(0 <= index && index < sDirLightNum);

	dirLights_[index].SetActive(active);
}

void LightGroup::SetDirLightDir(int index, const Vector3& lightdir)
{
	assert(0 <= index && index < sDirLightNum);

	dirLights_[index].SetLightDir(lightdir);
	dirty_ = true;
}

void LightGroup::SetDirLightColor(int index, const Vector3& lightcolor)
{
	assert(0 <= index && index < sDirLightNum);

	dirLights_[index].SetLightColor(lightcolor);
	dirty_ = true;
}

void LightGroup::SetPointLightActive(int index, bool active)
{
	assert(0 <= index && index < sPointLightNum);

	pointLights_[index].SetActive(active);
}

void LightGroup::SetPointLightPos(int index, const Vector3& lightpos)
{
	assert(0 <= index && index < sPointLightNum);

	pointLights_[index].SetLightPos(lightpos);
	dirty_ = true;
}

void LightGroup::SetPointLightColor(int index, const Vector3& lightcolor)
{
	assert(0 <= index && index < sPointLightNum);

	pointLights_[index].SetLightColor(lightcolor);
	dirty_ = true;
}

void LightGroup::SetPointLightAtten(int index, const Vector3& lightAtten)
{
	assert(0 <= index && index < sPointLightNum);

	pointLights_[index].SetLightAtten(lightAtten);
	dirty_ = true;
}

void LightGroup::SetPointLightIndensity(int index, const float lightIdensity)
{
	assert(0 <= index && index < sPointLightNum);

	pointLights_[index].SetIndensity(lightIdensity);
	dirty_ = true;
}

void LightGroup::SetPointLightRadius(int index, const float lightRadius)
{
	assert(0 <= index && index < sPointLightNum);

	pointLights_[index].SetRadius(lightRadius);
	dirty_ = true;
}

void LightGroup::SetPointLightDecay(int index, const float lightDecay)
{
	assert(0 <= index && index < sPointLightNum);

	pointLights_[index].SetDecay(lightDecay);
	dirty_ = true;
}

void LightGroup::SetPointLightDistance(int index, const float lightDistance)
{
	assert(0 <= index && index < sPointLightNum);

	pointLights_[index].SetDistance(lightDistance);
	dirty_ = true;
}

void LightGroup::SetSpotLightActive(int index, bool active)
{
	assert(0 <= index && index < sSpotLightNum);

	spotLights_[index].SetActive(active);
}

void LightGroup::SetSpotLightDir(int index, const Vector3& lightdir)
{
	assert(0 <= index && index < sSpotLightNum);

	spotLights_[index].SetLightDir(lightdir);
	dirty_ = true;
}

void LightGroup::SetSpotLightPos(int index, const Vector3& lightpos)
{
	assert(0 <= index && index < sSpotLightNum);

	spotLights_[index].SetLightPos(lightpos);
	dirty_ = true;
}

void LightGroup::SetSpotLightColor(int index, const Vector3& lightcolor)
{
	assert(0 <= index && index < sSpotLightNum);

	spotLights_[index].SetLightColor(lightcolor);
	dirty_ = true;
}

void LightGroup::SetSpotLightAtten(int index, const Vector3& lightAtten)
{
	assert(0 <= index && index < sSpotLightNum);

	spotLights_[index].SetLightAtten(lightAtten);
	dirty_ = true;
}

void LightGroup::SetSpotLightFactorAngle(int index, const Vector2& lightFactorAngle)
{
	assert(0 <= index && index < sSpotLightNum);

	spotLights_[index].SetLightFactorAngle(lightFactorAngle);
	dirty_ = true;
}

void LightGroup::SetCircleShadowActive(int index, bool active)
{
	assert(0 <= index && index < sCircleShadowNum);

	circleShadows_[index].SetActive(active);
}

void LightGroup::SetCircleShadowCasterPos(int index, const Vector3& casterPos)
{
	assert(0 <= index && index < sCircleShadowNum);

	circleShadows_[index].SetCasterPos(casterPos);
	dirty_ = true;
}

void LightGroup::SetCircleShadowDir(int index, const Vector3& lightdir)
{
	assert(0 <= index && index < sCircleShadowNum);

	circleShadows_[index].SetDir(lightdir);
	dirty_ = true;
}

void LightGroup::SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight)
{
	assert(0 <= index && index < sCircleShadowNum);

	circleShadows_[index].SetDistanceCasterLight(distanceCasterLight);
	dirty_ = true;
}

void LightGroup::SetCircleShadowAtten(int index, const Vector3& lightAtten)
{
	assert(0 <= index && index < sCircleShadowNum);

	circleShadows_[index].SetAtten(lightAtten);
	dirty_ = true;
}

void LightGroup::SetCircleShadowFactorAngle(int index, const Vector2& lightFactorAngle)
{
	assert(0 <= index && index < sCircleShadowNum);

	circleShadows_[index].SetFactorAngle(lightFactorAngle);
	dirty_ = true;
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
	result = sDevice->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

	// 定数バッファとのデータリンク
	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));
}

