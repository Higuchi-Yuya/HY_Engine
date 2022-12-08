#pragma once
#include<DirectXTex.h>
#include"DirectXCommon.h"
#include<wrl.h>
#include<array>
#include<string>

class Texture
{
public:// メンバ関数

	// テクスチャ読み込み
	static void LoadTexture(uint32_t index, const std::string& fileName);
	
	// テクスチャで一度必要な初期化
	static void StaticInitialize(DirectXCommon* dxcommon);
	
	// テクスチャに必要なコマンド設定
	void SetTextureCommands(uint32_t index);

public:// 静的メンバ変数
	// SRVの最大個数
	static const size_t kMaxSRVCount = 2056;
	// DirextXの基盤を借りてくる
	static DirectXCommon* dxcommon_;


private:// メンバ変数
	// テクスチャバッファ
	static std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxSRVCount>textureBuffers_;
	// SRVヒープ
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
};

