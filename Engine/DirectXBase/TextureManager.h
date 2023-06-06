#pragma once

#include"DirectXCommon.h"
#include<wrl.h>
#include<array>
#include<string>
#include <d3dx12.h>
#include <DirectXTex.h>
#include "Texture.h"
#include <map>

class TextureManager
{
public:// メンバ関数

	// テクスチャ読み込み
	static Texture Load2DTexture(const std::string& fileName = "NULL");

	static uint32_t LoadTexture(const std::string& fileName = "NULL");

	static uint32_t LoadTexture(const wchar_t* fileName);

	// テクスチャで一度必要な初期化
	static void StaticInitialize(DirectXCommon* dxcommon);

	static void CreateSRV(Texture& texture, ID3D12Resource* buffer);

public:// 静的メンバ変数
	static UINT srvIncrementIndex;

	// SRVの最大個数
	static const size_t kMaxSRVCount = 2056;

	// デフォルトテクスチャ2D格納ディレクトリ
	static std::string kDefault2DTextureDirectoryPath;

	// デフォルトテクスチャ格納ディレクトリ
	static std::string kDefaultTextureDirectoryPath;

	// DirextXの基盤を借りてくる
	static DirectXCommon* dxcommon_;

	// SRVヒープ
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;

	// テクスチャバッファ
	static std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxSRVCount>textureBuffers_;

	//// テクスチャ全体
	//static std::map<std::string, std::unique_ptr<Texture>> textureMap;

private:// プライベート関数
	//[[nodiscard]]
	ID3D12Resource* UpLoadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

	// 
	ID3D12Resource* CreateBufferResource(ID3D12Device* device, uint64_t intermediateSize);

	// コマンドを一回積んだあとに解放する関数
	static void ExcuteComandList();

private:// メンバ変数
	// テクスチャリソースデスク
	static D3D12_RESOURCE_DESC textureResourceDesc;


};

