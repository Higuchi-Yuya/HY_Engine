#include"TextureManager.h"

#include <DirectXTexD3D12.cpp>
using namespace DirectX;
using namespace Microsoft::WRL;

std::string TextureManager::sDefault2DTextureDirectoryPath = "Resources/2D_Resources/";
DirectXCommon* TextureManager::sDxcommon_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> TextureManager::sSrvHeap = nullptr;
D3D12_RESOURCE_DESC TextureManager::sTextureResourceDesc{};

uint32_t TextureManager::sSrvIncrementIndex = 1;



Texture* TextureManager::LoadFreeTexture(const std::string& filePath)
{
	// 作成するテクスチャ
	Texture* tex;
	tex = new Texture();

	// 結果確認
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// ディレクトリパスとファイル名を連結してフルパスを得る
	std::string fullPath = filePath;

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


	//リソース設定
	sTextureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	sTextureResourceDesc.Format = metadata.format;
	sTextureResourceDesc.Width = (UINT64)metadata.width;	// 幅
	sTextureResourceDesc.Height = (UINT)metadata.height;	// 高さ
	sTextureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	sTextureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	sTextureResourceDesc.SampleDesc.Count = 1;
	sTextureResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	sTextureResourceDesc.Alignment = 0;

	// テクスチャのサイズをセット
	tex->size = { (float)sTextureResourceDesc.Width, (float)sTextureResourceDesc.Height };

	// テクスチャバッファの生成

	result = sDxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,					// Heapの設定
			D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
			&sTextureResourceDesc,				// Resourceの設定
			D3D12_RESOURCE_STATE_COPY_DEST,		// データ転送される設定
			nullptr,							// Clear最適値。使わないのでNullptr
			IID_PPV_ARGS(&tex->buffer));			// 作成するResourceポインタへのポインタ

	assert(SUCCEEDED(result));

	// SRVヒープを作成
	CreateSRV(*tex, tex->buffer.Get());

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

		assert(SUCCEEDED(result));
	}

	uint64_t uploadSize = GetRequiredIntermediateSize(tex->buffer.Get(), 0, (UINT)metadata.mipLevels);

	// ヒープの設定
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC sTextureResourceDesc1 =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

	// テクスチャバッファの生成
	result = sDxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp1,
			D3D12_HEAP_FLAG_NONE,
			&sTextureResourceDesc1,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer));
	assert(SUCCEEDED(result));

	UpdateSubresources(
		sDxcommon_->GetCommandList(),
		tex->buffer.Get(),
		uploadBuffer.Get(),
		0,
		0,
		(UINT)metadata.mipLevels,
		subResourcesDatas.data());

	// Textureへの転送後は利用できるよう、D3D12_RESOUCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GEMEROC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER  barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = tex->buffer.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	sDxcommon_->GetCommandList()->ResourceBarrier(1, &barrier);
	ExcuteComandList();

	return tex;
	
}

Texture TextureManager::Load2DTexture(const std::string& fileName)
{
	// 作成するテクスチャ
	Texture tex;

	// 結果確認
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// ディレクトリパスとファイル名を連結してフルパスを得る
	std::string fullPath = sDefault2DTextureDirectoryPath + fileName;

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


	//リソース設定
	sTextureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	sTextureResourceDesc.Format = metadata.format;
	sTextureResourceDesc.Width = (UINT64)metadata.width;	// 幅
	sTextureResourceDesc.Height = (UINT)metadata.height;	// 高さ
	sTextureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	sTextureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	sTextureResourceDesc.SampleDesc.Count = 1;
	sTextureResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	sTextureResourceDesc.Alignment = 0;

	// テクスチャのサイズをセット
	tex.size = { (float)sTextureResourceDesc.Width, (float)sTextureResourceDesc.Height };

	// テクスチャバッファの生成
	
	result = sDxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,					// Heapの設定
			D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
			&sTextureResourceDesc,				// Resourceの設定
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

		assert(SUCCEEDED(result));
	}

	uint64_t uploadSize = GetRequiredIntermediateSize(tex.buffer.Get(), 0, (UINT)metadata.mipLevels);

	// ヒープの設定
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC sTextureResourceDesc1 =
	CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

	// テクスチャバッファの生成
	result = sDxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp1,
			D3D12_HEAP_FLAG_NONE,
			&sTextureResourceDesc1,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer));
	assert(SUCCEEDED(result));

	UpdateSubresources(
		sDxcommon_->GetCommandList(),
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

	sDxcommon_->GetCommandList()->ResourceBarrier(1, &barrier);
	ExcuteComandList();

	return tex;
}

Texture* TextureManager::Load2DTextureP(const std::string fileName)
{
	// 作成するテクスチャ
	Texture* tex;
	tex = new Texture();

	// 結果確認
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// ディレクトリパスとファイル名を連結してフルパスを得る
	std::string fullPath = sDefault2DTextureDirectoryPath + fileName;

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


	//リソース設定
	sTextureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	sTextureResourceDesc.Format = metadata.format;
	sTextureResourceDesc.Width = (UINT64)metadata.width;	// 幅
	sTextureResourceDesc.Height = (UINT)metadata.height;	// 高さ
	sTextureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	sTextureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	sTextureResourceDesc.SampleDesc.Count = 1;
	sTextureResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	sTextureResourceDesc.Alignment = 0;

	// テクスチャのサイズをセット
	tex->size = { (float)sTextureResourceDesc.Width, (float)sTextureResourceDesc.Height };

	// テクスチャバッファの生成

	result = sDxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,					// Heapの設定
			D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
			&sTextureResourceDesc,				// Resourceの設定
			D3D12_RESOURCE_STATE_COPY_DEST,		// データ転送される設定
			nullptr,							// Clear最適値。使わないのでNullptr
			IID_PPV_ARGS(&tex->buffer));			// 作成するResourceポインタへのポインタ

	assert(SUCCEEDED(result));

	// SRVヒープを作成
	CreateSRV(*tex, tex->buffer.Get());

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

		assert(SUCCEEDED(result));
	}

	uint64_t uploadSize = GetRequiredIntermediateSize(tex->buffer.Get(), 0, (UINT)metadata.mipLevels);

	// ヒープの設定
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC sTextureResourceDesc1 =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

	// テクスチャバッファの生成
	result = sDxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp1,
			D3D12_HEAP_FLAG_NONE,
			&sTextureResourceDesc1,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer));
	assert(SUCCEEDED(result));

	UpdateSubresources(
		sDxcommon_->GetCommandList(),
		tex->buffer.Get(),
		uploadBuffer.Get(),
		0,
		0,
		(UINT)metadata.mipLevels,
		subResourcesDatas.data());

	// Textureへの転送後は利用できるよう、D3D12_RESOUCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GEMEROC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER  barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = tex->buffer.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	sDxcommon_->GetCommandList()->ResourceBarrier(1, &barrier);
	ExcuteComandList();

	return tex;
}

Texture TextureManager::LoadTexture(std::string fileName)
{
	// 作成するテクスチャ
	Texture tex;

	// 結果確認
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// ディレクトリパスとファイル名を連結してフルパスを得る
	std::string fullPath = fileName;

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
	textureHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);


	//リソース設定
	sTextureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	sTextureResourceDesc.Format = metadata.format;
	sTextureResourceDesc.Width = (UINT64)metadata.width;	// 幅
	sTextureResourceDesc.Height = (UINT)metadata.height;	// 高さ
	sTextureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	sTextureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	sTextureResourceDesc.SampleDesc.Count = 1;
	sTextureResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	sTextureResourceDesc.Alignment = 0;

	// テクスチャのサイズをセット
	tex.size = { (float)sTextureResourceDesc.Width, (float)sTextureResourceDesc.Height };

	// テクスチャバッファの生成

	result = sDxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,					// Heapの設定
			D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
			&sTextureResourceDesc,				// Resourceの設定
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

		assert(SUCCEEDED(result));
	}

	uint64_t uploadSize = GetRequiredIntermediateSize(tex.buffer.Get(), 0, (UINT)metadata.mipLevels);

	// ヒープの設定
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC sTextureResourceDesc1 =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

	// テクスチャバッファの生成
	result = sDxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp1,
			D3D12_HEAP_FLAG_NONE,
			&sTextureResourceDesc1,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer));
	assert(SUCCEEDED(result));

	UpdateSubresources(
		sDxcommon_->GetCommandList(),
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

	sDxcommon_->GetCommandList()->ResourceBarrier(1, &barrier);
	ExcuteComandList();

	return tex;
}

Texture* TextureManager::LoadTextureP(std::string fileName)
{
	// 作成するテクスチャ
	Texture *tex;
	tex = new Texture();

	// 結果確認
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// ディレクトリパスとファイル名を連結してフルパスを得る
	std::string fullPath = fileName;

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
	textureHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);


	//リソース設定
	sTextureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	sTextureResourceDesc.Format = metadata.format;
	sTextureResourceDesc.Width = (UINT64)metadata.width;	// 幅
	sTextureResourceDesc.Height = (UINT)metadata.height;	// 高さ
	sTextureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	sTextureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	sTextureResourceDesc.SampleDesc.Count = 1;
	sTextureResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	sTextureResourceDesc.Alignment = 0;

	// テクスチャのサイズをセット
	tex->size = { (float)sTextureResourceDesc.Width, (float)sTextureResourceDesc.Height };

	// テクスチャバッファの生成

	result = sDxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,					// Heapの設定
			D3D12_HEAP_FLAG_NONE,				// Heapの特殊な設定。特になし。
			&sTextureResourceDesc,				// Resourceの設定
			D3D12_RESOURCE_STATE_COPY_DEST,		// データ転送される設定
			nullptr,							// Clear最適値。使わないのでNullptr
			IID_PPV_ARGS(&tex->buffer));			// 作成するResourceポインタへのポインタ

	assert(SUCCEEDED(result));

	// SRVヒープを作成
	CreateSRV(*tex, tex->buffer.Get());

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

		assert(SUCCEEDED(result));
	}

	uint64_t uploadSize = GetRequiredIntermediateSize(tex->buffer.Get(), 0, (UINT)metadata.mipLevels);

	// ヒープの設定
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC sTextureResourceDesc1 =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

	// テクスチャバッファの生成
	result = sDxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp1,
			D3D12_HEAP_FLAG_NONE,
			&sTextureResourceDesc1,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&uploadBuffer));
	assert(SUCCEEDED(result));

	UpdateSubresources(
		sDxcommon_->GetCommandList(),
		tex->buffer.Get(),
		uploadBuffer.Get(),
		0,
		0,
		(UINT)metadata.mipLevels,
		subResourcesDatas.data());

	// Textureへの転送後は利用できるよう、D3D12_RESOUCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GEMEROC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER  barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = tex->buffer.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	sDxcommon_->GetCommandList()->ResourceBarrier(1, &barrier);
	ExcuteComandList();

	return tex;
}

void TextureManager::StaticInitialize(DirectXCommon* dxcommon)
{
	sDxcommon_ = dxcommon;

	// 結果確認
	HRESULT result;

	// デスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC sSrvHeapDesc = {};
	sSrvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	sSrvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	sSrvHeapDesc.NumDescriptors = sMaxSRVCount;

	// 設定をもとにSRV用デスクリプタヒープを生成
	result = sDxcommon_->GetDevice()->CreateDescriptorHeap(&sSrvHeapDesc,
		IID_PPV_ARGS(&sSrvHeap));
	assert(SUCCEEDED(result));
}

void TextureManager::StaticFinalize()
{
	sSrvHeap = nullptr;
}

void TextureManager::CreateSRV(Texture& texture, ID3D12Resource* buffer)
{
	// SRVヒープの先頭ハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle = sSrvHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = sSrvHeap->GetGPUDescriptorHandleForHeapStart();

	uint32_t descriptorSize = sDxcommon_->GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	srvCpuHandle.ptr += (SIZE_T)(descriptorSize * sSrvIncrementIndex);
	srvGpuHandle.ptr += (SIZE_T)(descriptorSize * sSrvIncrementIndex);

	texture.SetCpuHandle(srvCpuHandle);
	texture.SetGpuHandle(srvGpuHandle);

	// シェーダーリソースビュー設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	// srv設定構造体
	srvDesc.Format = buffer->GetDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = sTextureResourceDesc.MipLevels;

	// ハンドルの指す位置にシェーダーリソースビュー作成
	sDxcommon_->GetDevice()->
		CreateShaderResourceView(buffer, &srvDesc, srvCpuHandle);

	sSrvIncrementIndex++;
}

void TextureManager::ExcuteComandList()
{
	ID3D12GraphicsCommandList* iCommandList = sDxcommon_->GetCommandList();

	iCommandList->Close();

	ID3D12CommandQueue* iCommandQueue = sDxcommon_->GetCommandQueue();

	ID3D12CommandList* list[] = { iCommandList };
	iCommandQueue->ExecuteCommandLists(1, list);

	sDxcommon_->PreIncrimentFenceValue();

	// コマンドの実行完了を待つ
	iCommandQueue->Signal(sDxcommon_->GetFence(), sDxcommon_->GetFenceValue());

	auto test = sDxcommon_->GetFence()->GetCompletedValue();
	if (sDxcommon_->GetFence()->GetCompletedValue() != sDxcommon_->GetFenceValue())
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		sDxcommon_->GetFence()->SetEventOnCompletion(sDxcommon_->GetFenceValue(), event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	HRESULT result;

	// キューをクリア
	result = sDxcommon_->GetCommandAllocator()->Reset();
	assert(SUCCEEDED(result));
	// 再びコマンドリストを貯める準備
	result = iCommandList->Reset(sDxcommon_->GetCommandAllocator(), nullptr);
	assert(SUCCEEDED(result));
}
