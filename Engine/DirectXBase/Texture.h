#pragma once
#include<DirectXTex.h>
#include"DirectXCommon.h"
#include<wrl.h>
#include<array>
#include<string>
#include <d3dx12.h>
class Texture
{
public:// メンバ関数

	// テクスチャ読み込み
	static uint32_t LoadTexture(const std::string& fileName = "NULL");
	
	static uint32_t LoadTexture(const wchar_t* fileName);

	// テクスチャで一度必要な初期化
	static void StaticInitialize(DirectXCommon* dxcommon);

public:// 静的メンバ変数
	// SRVの最大個数
	static const size_t kMaxSRVCount = 2056;
	// デフォルトテクスチャ格納ディレクトリ
	static std::string kDefaultTextureDirectoryPath;

	// DirextXの基盤を借りてくる
	static DirectXCommon* dxcommon_;

	// SRVヒープ
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;

	// テクスチャバッファ
	static std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxSRVCount>textureBuffers_;

private:// メンバ変数
	// テクスチャリソースデスク
	static D3D12_RESOURCE_DESC textureResourceDesc;
};

