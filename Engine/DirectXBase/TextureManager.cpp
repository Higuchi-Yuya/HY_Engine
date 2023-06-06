#include"TextureManager.h"

#include <DirectXTexD3D12.cpp>
using namespace DirectX;
using namespace Microsoft::WRL;

std::string TextureManager::kDefault2DTextureDirectoryPath = "Resources/2D_Resources/";
std::string TextureManager::kDefaultTextureDirectoryPath = "Resources/";
DirectXCommon* TextureManager::dxcommon_ = nullptr;
std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, TextureManager::kMaxSRVCount> TextureManager::textureBuffers_;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> TextureManager::srvHeap = nullptr;
D3D12_RESOURCE_DESC TextureManager::textureResourceDesc{};

UINT TextureManager::srvIncrementIndex = 1;
//std::map<std::string, std::unique_ptr<Texture>> TextureManager::textureMap;

Texture TextureManager::Load2DTexture(const std::string& fileName)
{
	// 作成するテクスチャ
	Texture tex;

	// 結果確認
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// ディレクトリパスとファイル名を連結してフルパスを得る
	std::string fullPath = kDefault2DTextureDirectoryPath + fileName;

	// ワイド文字列に変換した際の文字列バッファサイズを計算
	int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, nullptr, 0);

	// ワイド文字列に変換
	std::vector<wchar_t>wfilePath(filePathBufferSize);
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath.data(), filePathBufferSize);


	result = LoadFromWICFile(
		wfilePath.data(),
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	ScratchImage mipChain{};
	// ミップマップ生成
	result = GenerateMipMaps(
		scratchImg.GetImages(),
		scratchImg.GetImageCount(),
		scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);

	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// 読み込んだディフューズテクスチャをSRGBとして扱う
	metadata.format = MakeSRGB(metadata.format);


	//ヒープ設定
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	//textureHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//textureHeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	//textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;

	//リソース設定
	CD3DX12_RESOURCE_DESC textureResourceDesc= 
		CD3DX12_RESOURCE_DESC::Tex2D(
		metadata.format,
		(UINT64)metadata.width,
		(UINT)metadata.height,
		(UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels,
		1);
	//textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//textureResourceDesc.Format = metadata.format;
	//textureResourceDesc.Width = (UINT64)metadata.width;	// 幅
	//textureResourceDesc.Height = (UINT)metadata.height;	// 高さ
	//textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	//textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	//textureResourceDesc.SampleDesc.Count = 1;
	//textureResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	//textureResourceDesc.Alignment = 0;

	// テクスチャのサイズをセット
	tex.size = { (float)textureResourceDesc.Width, (float)textureResourceDesc.Height };

	// テクスチャバッファの生成
	
	result = dxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,					// Heapの設定
			D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
			&textureResourceDesc,				// Resourceの設定
			D3D12_RESOURCE_STATE_COPY_DEST,		// データ転送される設定
			nullptr,							// Clear最適値。使わないのでNullptr
			IID_PPV_ARGS(&tex.buffer));			// 作成するResourceポインタへのポインタ

	assert(SUCCEEDED(result));

	// SRVヒープを作成
	CreateSRV(tex, tex.buffer.Get());

	// サブリソースを作成
	std::vector<D3D12_SUBRESOURCE_DATA> subResourcesDatas{};
	subResourcesDatas.resize(metadata.mipLevels);

	//全ミップマップについて
	for (size_t i = 0; i < subResourcesDatas.size(); i++) {
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		subResourcesDatas[i].pData = img->pixels;
		subResourcesDatas[i].RowPitch = (UINT)img->rowPitch;
		subResourcesDatas[i].SlicePitch = (UINT)img->slicePitch;

		//result = tex.buffer->WriteToSubresource(
		//	(UINT)i,
		//	nullptr,				//全領域へコピー
		//	img->pixels,			//元データアドレス
		//	(UINT)img->rowPitch,	//1ラインサイズ
		//	(UINT)img->slicePitch	//全サイズ
		//);
		assert(SUCCEEDED(result));
	}

	uint64_t uploadSize = GetRequiredIntermediateSize(tex.buffer.Get(), 0, (UINT)metadata.mipLevels);

	//// シェーダーリソースビュー設定
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//srvDesc.Format = textureResourceDesc.Format;//RGBA float
	//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	//srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	// ヒープの設定
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC textureResourceDesc1 =
	CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

	// テクスチャバッファの生成
	result = dxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp1,
			D3D12_HEAP_FLAG_NONE,
			&textureResourceDesc1,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer));
	assert(SUCCEEDED(result));


	UpdateSubresources(
		dxcommon_->GetCommandList(),
		tex.buffer.Get(),
		uploadBuffer.Get(),
		0,
		0,
		(UINT)metadata.mipLevels,
		subResourcesDatas.data());

	// Textureへの転送後は利用できるよう、D3D12_RESOUCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GEMEROC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER  barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = tex.buffer.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	dxcommon_->GetCommandList()->ResourceBarrier(1, &barrier);
	ExcuteComandList();

	return tex;
}

uint32_t TextureManager::LoadTexture(const std::string& fileName)
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

	// ディレクトリパスとファイル名を連結してフルパスを得る
	std::string fullPath = kDefaultTextureDirectoryPath + fileName;

	// ワイド文字列に変換した際の文字列バッファサイズを計算
	int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, nullptr, 0);

	// ワイド文字列に変換
	std::vector<wchar_t>wfilePath(filePathBufferSize);
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath.data(), filePathBufferSize);


	result = LoadFromWICFile(
		wfilePath.data(),
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
		IID_PPV_ARGS(&texBuff));

	//全ミップマップについて
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = texBuff->WriteToSubresource(
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
			dxcommon_->GetDevice()->CreateShaderResourceView(texBuff.Get(), &srvDesc, srvHandle);
			//iを戻り値として返す
			return i;
		}
	}

}

uint32_t TextureManager::LoadTexture(const wchar_t* fileName)
{
	// 結果確認
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	result = LoadFromWICFile(
		fileName,
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
		IID_PPV_ARGS(&texBuff));

	//全ミップマップについて
	for (size_t i = 0; i < metadata.mipLevels; i++) {
		//ミップマップレベルを指定してイメージを取得
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//テクスチャバッファにデータ転送
		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,				//全領域へコピー
			img->pixels,			//元データアドレス
			(UINT)img->rowPitch,	//1ラインサイズ
			(UINT)img->slicePitch	//全サイズ
		);
		assert(SUCCEEDED(result));
	}

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
			dxcommon_->GetDevice()->CreateShaderResourceView(texBuff.Get(), &srvDesc, srvHandle);
			//iを戻り値として返す
			return i;
		}
	}
}

void TextureManager::StaticInitialize(DirectXCommon* dxcommon)
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

void TextureManager::CreateSRV(Texture& texture, ID3D12Resource* buffer)
{
	// SRVヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();

	UINT descriptorSize = dxcommon_->GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	srvCpuHandle.ptr += (SIZE_T)(descriptorSize * srvIncrementIndex);
	srvGpuHandle.ptr += (SIZE_T)(descriptorSize * srvIncrementIndex);

	texture.SetCpuHandle(srvCpuHandle);
	texture.SetGpuHandle(srvGpuHandle);

	// シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	// srv設定構造体
	srvDesc.Format = buffer->GetDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;	// 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = buffer->GetDesc().MipLevels;

	// ハンドルの指す位置にシェーダーリソースビュー作成
	dxcommon_->GetDevice()->
		CreateShaderResourceView(buffer, &srvDesc, srvCpuHandle);

	srvIncrementIndex++;
}

[[nodiscard]]
ID3D12Resource* TextureManager::UpLoadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	std::vector<D3D12_SUBRESOURCE_DATA>subresources;
	PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
	UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
	// Textureへの転送後は利用できるよう、D3D12_RESOUCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GEMEROC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);

	return intermediateResource;
}

ID3D12Resource* TextureManager::CreateBufferResource(ID3D12Device* device, uint64_t intermediateSize)
{


	return nullptr;
}

void TextureManager::ExcuteComandList()
{
	ID3D12GraphicsCommandList* iCommandList = dxcommon_->GetCommandList();

	iCommandList->Close();

	ID3D12CommandQueue* iCommandQueue = dxcommon_->GetCommandQueue();

	ID3D12CommandList* list[] = { iCommandList };
	iCommandQueue->ExecuteCommandLists(1, list);

	dxcommon_->PreIncrimentFenceValue();

	// コマンドの実行完了を待つ
	iCommandQueue->Signal(dxcommon_->GetFence(), dxcommon_->GetFenceValue());

	auto test = dxcommon_->GetFence()->GetCompletedValue();
	if (dxcommon_->GetFence()->GetCompletedValue() != dxcommon_->GetFenceValue())
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		dxcommon_->GetFence()->SetEventOnCompletion(dxcommon_->GetFenceValue(), event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	HRESULT result;

	// キューをクリア
	result = dxcommon_->GetCommandAllocator()->Reset();
	assert(SUCCEEDED(result));
	// 再びコマンドリストを貯める準備
	result = iCommandList->Reset(dxcommon_->GetCommandAllocator(), nullptr);
	assert(SUCCEEDED(result));
}
