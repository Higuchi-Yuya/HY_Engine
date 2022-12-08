#include "Texture.h"
using namespace DirectX;
using namespace Microsoft::WRL;

DirectXCommon* Texture::dxcommon_ = nullptr;
std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, Texture::kMaxSRVCount> Texture::textureBuffers_;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> Texture::srvHeap;

void Texture::LoadTexture(uint32_t index, const std::string& fileName)
{
	// 結果確認
	HRESULT result;

	//// 横方向ぷくセル数
//const size_t textureWidth = 256;
//// 縦方向ピクセル数
//const size_t textureHeight = 256;
//// 配列の要素数
//const size_t imageDataCount = textureWidth * textureHeight;
//// 画像イメージデータ配列
//XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];// 必ず後で開放する

//// 全ピクセルの色の初期化
//for (size_t i = 0; i < imageDataCount; i++) {
//	imageData[i].x = 1.0f;  // R
//	imageData[i].y = 0.0f;  // G
//	imageData[i].z = 0.0f;  // B
//	imageData[i].w = 1.0f;  // A
//}

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		L"Resources/texture.png",
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	ScratchImage mipChain{};
	// ミップマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);


	//ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//リソース設定
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;	// 幅
	textureResourceDesc.Height = (UINT)metadata.height;	// 高さ
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;

	// テクスチャバッファの生成
	ComPtr<ID3D12Resource> texBuff;
	result = dxcommon_->GetDevice()->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&textureBuffers_[index]));

	//全ミップマップについて
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = textureBuffers_[index]->WriteToSubresource(
			(UINT)i,
			nullptr,				//全領域へコピー
			img->pixels,			//元データアドレス
			(UINT)img->rowPitch,	//1ラインサイズ
			(UINT)img->slicePitch	//全サイズ
		);
		assert(SUCCEEDED(result));
	}

	//テクスチャバッファにデータ転送
	//result = texBuff->WriteToSubresource(
	//	0,
	//	nullptr,				//全領域へコピー
	//	imageData,			//元データアドレス
	//	sizeof(XMFLOAT4) * textureWidth,	//1ラインサイズ
	//	sizeof(XMFLOAT4) * imageDataCount 	//全サイズ
	//);

	//// 元データ解放
	//delete[] imageData;
	
	// シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureResourceDesc.Format;//RGBA float
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	for (int i = 0; i < textureBuffers_.size(); i++) {

		//まだテクスチャ情報が割り当てられていないところにテクスチャ情報を入れる
		if (!textureBuffers_[i]) {
			textureBuffers_[i] = texBuff;
			UINT incrementSize = dxcommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			// SRVヒープの先頭ハンドルを取得
			D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
			srvHandle.ptr += incrementSize * i;
			dxcommon_->GetDevice()->CreateShaderResourceView(textureBuffers_[i].Get(), &srvDesc, srvHandle);
		}
	}
	
}

void Texture::StaticInitialize(DirectXCommon* dxcommon)
{
	dxcommon_ = dxcommon;

	// 結果確認
	HRESULT result;

	// デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	// 設定をもとにSRV用デスクリプタヒープを生成
	result = dxcommon_->GetDevice()->CreateDescriptorHeap(&srvHeapDesc,
		IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));
}

void Texture::SetTextureCommands(uint32_t index)
{
	// SRVヒープの設定コマンド
	dxcommon_->GetCommandList()->SetDescriptorHeaps(1, &srvHeap);

	// SRVヒープの先頭ハンドルを取得
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
	// 一つ分のハンドル
	UINT incrementSize = dxcommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	// index分のハンドルをずらす
	srvGpuHandle.ptr += incrementSize * index;

	// SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
	dxcommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
}
