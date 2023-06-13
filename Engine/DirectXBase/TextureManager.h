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

	// 2Dのテクスチャの読み込み
	static Texture Load2DTexture(const std::string& fileName = "NULL");
	static Texture* Load2DTextureP(const std::string fileName);

	// モデルのマテリアルテクスチャの読み込み
	static Texture LoadTexture(std::string fileName);
	static Texture* LoadTextureP(std::string fileName);

	// テクスチャで一度必要な初期化
	static void StaticInitialize(DirectXCommon* dxcommon);

public:// 静的メンバ変数
	static uint32_t sSrvIncrementIndex;

	// SRVの最大個数
	static const size_t sMaxSRVCount = 2056;

	// デフォルトテクスチャ2D格納ディレクトリ
	static std::string sDefault2DTextureDirectoryPath;

	// デフォルトテクスチャ格納ディレクトリ
	static std::string sDefaultTextureDirectoryPath;

	// DirextXの基盤を借りてくる
	static DirectXCommon* sDxcommon_;

	// SRVヒープ
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> sSrvHeap;

private:// プライベート関数
	// シェーダリソースビューの作成
	static void CreateSRV(Texture& texture, ID3D12Resource* buffer);

	// コマンドを一回積んだあとに解放する関数
	static void ExcuteComandList();

private:// メンバ変数
	// テクスチャリソースデスク
	static D3D12_RESOURCE_DESC sTextureResourceDesc;
};

