#include "Material.h"
#include "TextureManager.h"

using namespace std;
/// <summary>
/// 静的メンバ変数の実体
/// </summary>
ID3D12Device* Material::device_ = nullptr;


void Material::StaticInitialize(ID3D12Device* device)
{
	Material::device_ = device;
}

Material* Material::Create()
{
	Material* instance = new Material;

	instance->Initialize();

	return instance;
}

void Material::LoadTexture(const std::string& directoryPath, const std::string& filename)
{
	// ファイルパスを結合
	string filepath = directoryPath + filename;

	//// ユニコード文字列に変換する
	//wchar_t wfilepath[128];
	//int iBufferSize = MultiByteToWideChar(CP_ACP, 0, filepath.c_str(), -1, wfilepath, _countof(wfilepath));

	textureIndex_ = TextureManager::LoadTexture(filepath);

	
}

void Material::Update()
{
	constMap_->ambient = ambient;
	constMap_->diffuse = diffuse;
	constMap_->specular = specular;

	constMap_->alpha = alpha;
	constBuff_->Unmap(0, nullptr);
}

void Material::Initialize()
{
	// 定数バッファの生成
	CreateConstantBuffer();
}

void Material::CreateConstantBuffer()
{
	HRESULT result;

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff);

	// マテリアル用定数バッファの生成
	result = device_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));

	assert(SUCCEEDED(result));

	// マテリアル用定数バッファへデータ転送
	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));
}
