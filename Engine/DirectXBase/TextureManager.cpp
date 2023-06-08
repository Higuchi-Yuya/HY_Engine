#include"TextureManager.h"

#include <DirectXTexD3D12.cpp>
using namespace DirectX;
using namespace Microsoft::WRL;

std::string TextureManager::kDefault2DTextureDirectoryPath = "Resources/2D_Resources/";
std::string TextureManager::kDefaultTextureDirectoryPath = "Resources/";
DirectXCommon* TextureManager::dxcommon_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> TextureManager::srvHeap = nullptr;
D3D12_RESOURCE_DESC TextureManager::textureResourceDesc{};

UINT TextureManager::srvIncrementIndex = 1;

Texture TextureManager::Load2DTexture(const std::string& fileName)
{
	// �쐬����e�N�X�`��
	Texture tex;

	// ���ʊm�F
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// �f�B���N�g���p�X�ƃt�@�C������A�����ăt���p�X�𓾂�
	std::string fullPath = kDefault2DTextureDirectoryPath + fileName;

	// ���C�h������ɕϊ������ۂ̕�����o�b�t�@�T�C�Y���v�Z
	int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, nullptr, 0);

	// ���C�h������ɕϊ�
	std::vector<wchar_t>wfilePath(filePathBufferSize);
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath.data(), filePathBufferSize);


	result = LoadFromWICFile(
		wfilePath.data(),
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	ScratchImage mipChain{};
	// �~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg.GetImages(),
		scratchImg.GetImageCount(),
		scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);

	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// �ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);


	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);


	//���\�[�X�ݒ�
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = (UINT64)metadata.width;	// ��
	textureResourceDesc.Height = (UINT)metadata.height;	// ����
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;
	textureResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	textureResourceDesc.Alignment = 0;

	// �e�N�X�`���̃T�C�Y���Z�b�g
	tex.size = { (float)textureResourceDesc.Width, (float)textureResourceDesc.Height };

	// �e�N�X�`���o�b�t�@�̐���
	
	result = dxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,					// Heap�̐ݒ�
			D3D12_HEAP_FLAG_NONE,				// Heap�̓���Ȑݒ�B���ɂȂ��B
			&textureResourceDesc,				// Resource�̐ݒ�
			D3D12_RESOURCE_STATE_COPY_DEST,		// �f�[�^�]�������ݒ�
			nullptr,							// Clear�œK�l�B�g��Ȃ��̂�Nullptr
			IID_PPV_ARGS(&tex.buffer));			// �쐬����Resource�|�C���^�ւ̃|�C���^

	assert(SUCCEEDED(result));

	// SRV�q�[�v���쐬
	CreateSRV(tex, tex.buffer.Get());

	// �T�u���\�[�X���쐬
	std::vector<D3D12_SUBRESOURCE_DATA> subResourcesDatas{};
	subResourcesDatas.resize(metadata.mipLevels);

	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < subResourcesDatas.size(); i++) {
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		subResourcesDatas[i].pData = img->pixels;
		subResourcesDatas[i].RowPitch = (UINT)img->rowPitch;
		subResourcesDatas[i].SlicePitch = (UINT)img->slicePitch;

		assert(SUCCEEDED(result));
	}

	uint64_t uploadSize = GetRequiredIntermediateSize(tex.buffer.Get(), 0, (UINT)metadata.mipLevels);

	// �q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC textureResourceDesc1 =
	CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

	// �e�N�X�`���o�b�t�@�̐���
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

	// Texture�ւ̓]����͗��p�ł���悤�AD3D12_RESOUCE_STATE_COPY_DEST����D3D12_RESOURCE_STATE_GEMEROC_READ��ResourceState��ύX����
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

Texture* TextureManager::Load2DTextureP(const std::string fileName)
{
	// �쐬����e�N�X�`��
	Texture* tex;
	tex = new Texture();

	// ���ʊm�F
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// �f�B���N�g���p�X�ƃt�@�C������A�����ăt���p�X�𓾂�
	std::string fullPath = kDefault2DTextureDirectoryPath + fileName;

	// ���C�h������ɕϊ������ۂ̕�����o�b�t�@�T�C�Y���v�Z
	int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, nullptr, 0);

	// ���C�h������ɕϊ�
	std::vector<wchar_t>wfilePath(filePathBufferSize);
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath.data(), filePathBufferSize);


	result = LoadFromWICFile(
		wfilePath.data(),
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	ScratchImage mipChain{};
	// �~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg.GetImages(),
		scratchImg.GetImageCount(),
		scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);

	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}

	// �ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);


	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);


	//���\�[�X�ݒ�
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = (UINT64)metadata.width;	// ��
	textureResourceDesc.Height = (UINT)metadata.height;	// ����
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;
	textureResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	textureResourceDesc.Alignment = 0;

	// �e�N�X�`���̃T�C�Y���Z�b�g
	tex->size = { (float)textureResourceDesc.Width, (float)textureResourceDesc.Height };

	// �e�N�X�`���o�b�t�@�̐���

	result = dxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,					// Heap�̐ݒ�
			D3D12_HEAP_FLAG_NONE,				// Heap�̓���Ȑݒ�B���ɂȂ��B
			&textureResourceDesc,				// Resource�̐ݒ�
			D3D12_RESOURCE_STATE_COPY_DEST,		// �f�[�^�]�������ݒ�
			nullptr,							// Clear�œK�l�B�g��Ȃ��̂�Nullptr
			IID_PPV_ARGS(&tex->buffer));			// �쐬����Resource�|�C���^�ւ̃|�C���^

	assert(SUCCEEDED(result));

	// SRV�q�[�v���쐬
	CreateSRV(*tex, tex->buffer.Get());

	// �T�u���\�[�X���쐬
	std::vector<D3D12_SUBRESOURCE_DATA> subResourcesDatas{};
	subResourcesDatas.resize(metadata.mipLevels);

	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < subResourcesDatas.size(); i++) {
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		subResourcesDatas[i].pData = img->pixels;
		subResourcesDatas[i].RowPitch = (UINT)img->rowPitch;
		subResourcesDatas[i].SlicePitch = (UINT)img->slicePitch;

		assert(SUCCEEDED(result));
	}

	uint64_t uploadSize = GetRequiredIntermediateSize(tex->buffer.Get(), 0, (UINT)metadata.mipLevels);

	// �q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC textureResourceDesc1 =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

	// �e�N�X�`���o�b�t�@�̐���
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
		tex->buffer.Get(),
		uploadBuffer.Get(),
		0,
		0,
		(UINT)metadata.mipLevels,
		subResourcesDatas.data());

	// Texture�ւ̓]����͗��p�ł���悤�AD3D12_RESOUCE_STATE_COPY_DEST����D3D12_RESOURCE_STATE_GEMEROC_READ��ResourceState��ύX����
	D3D12_RESOURCE_BARRIER  barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = tex->buffer.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	dxcommon_->GetCommandList()->ResourceBarrier(1, &barrier);
	ExcuteComandList();

	return tex;
}

Texture TextureManager::LoadTexture(std::string fileName)
{
	// �쐬����e�N�X�`��
	Texture tex;

	// ���ʊm�F
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// �f�B���N�g���p�X�ƃt�@�C������A�����ăt���p�X�𓾂�
	std::string fullPath = fileName;

	// ���C�h������ɕϊ������ۂ̕�����o�b�t�@�T�C�Y���v�Z
	int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, nullptr, 0);

	// ���C�h������ɕϊ�
	std::vector<wchar_t>wfilePath(filePathBufferSize);
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath.data(), filePathBufferSize);

	result = LoadFromWICFile(
		wfilePath.data(),
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	ScratchImage mipChain{};
	// �~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	// �ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);


	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);


	//���\�[�X�ݒ�
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = (UINT64)metadata.width;	// ��
	textureResourceDesc.Height = (UINT)metadata.height;	// ����
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;
	textureResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	textureResourceDesc.Alignment = 0;

	// �e�N�X�`���̃T�C�Y���Z�b�g
	tex.size = { (float)textureResourceDesc.Width, (float)textureResourceDesc.Height };

	// �e�N�X�`���o�b�t�@�̐���

	result = dxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,					// Heap�̐ݒ�
			D3D12_HEAP_FLAG_NONE,				// Heap�̓���Ȑݒ�B���ɂȂ��B
			&textureResourceDesc,				// Resource�̐ݒ�
			D3D12_RESOURCE_STATE_COPY_DEST,		// �f�[�^�]�������ݒ�
			nullptr,							// Clear�œK�l�B�g��Ȃ��̂�Nullptr
			IID_PPV_ARGS(&tex.buffer));			// �쐬����Resource�|�C���^�ւ̃|�C���^

	assert(SUCCEEDED(result));

	// SRV�q�[�v���쐬
	CreateSRV(tex, tex.buffer.Get());

	// �T�u���\�[�X���쐬
	std::vector<D3D12_SUBRESOURCE_DATA> subResourcesDatas{};
	subResourcesDatas.resize(metadata.mipLevels);

	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < subResourcesDatas.size(); i++) {
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		subResourcesDatas[i].pData = img->pixels;
		subResourcesDatas[i].RowPitch = (UINT)img->rowPitch;
		subResourcesDatas[i].SlicePitch = (UINT)img->slicePitch;

		assert(SUCCEEDED(result));
	}

	uint64_t uploadSize = GetRequiredIntermediateSize(tex.buffer.Get(), 0, (UINT)metadata.mipLevels);

	// �q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC textureResourceDesc1 =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

	// �e�N�X�`���o�b�t�@�̐���
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

	// Texture�ւ̓]����͗��p�ł���悤�AD3D12_RESOUCE_STATE_COPY_DEST����D3D12_RESOURCE_STATE_GEMEROC_READ��ResourceState��ύX����
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

Texture* TextureManager::LoadTextureP(std::string fileName)
{
	// �쐬����e�N�X�`��
	Texture *tex;
	tex = new Texture();

	// ���ʊm�F
	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};

	// �f�B���N�g���p�X�ƃt�@�C������A�����ăt���p�X�𓾂�
	std::string fullPath = fileName;

	// ���C�h������ɕϊ������ۂ̕�����o�b�t�@�T�C�Y���v�Z
	int filePathBufferSize = MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, nullptr, 0);

	// ���C�h������ɕϊ�
	std::vector<wchar_t>wfilePath(filePathBufferSize);
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath.data(), filePathBufferSize);

	result = LoadFromWICFile(
		wfilePath.data(),
		WIC_FLAGS_NONE,
		&metadata, scratchImg);

	ScratchImage mipChain{};
	// �~�b�v�}�b�v����
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	// �ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);


	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);


	//���\�[�X�ݒ�
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = (UINT64)metadata.width;	// ��
	textureResourceDesc.Height = (UINT)metadata.height;	// ����
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;
	textureResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	textureResourceDesc.Alignment = 0;

	// �e�N�X�`���̃T�C�Y���Z�b�g
	tex->size = { (float)textureResourceDesc.Width, (float)textureResourceDesc.Height };

	// �e�N�X�`���o�b�t�@�̐���

	result = dxcommon_->GetDevice()->
		CreateCommittedResource(
			&textureHeapProp,					// Heap�̐ݒ�
			D3D12_HEAP_FLAG_NONE,				// Heap�̓���Ȑݒ�B���ɂȂ��B
			&textureResourceDesc,				// Resource�̐ݒ�
			D3D12_RESOURCE_STATE_COPY_DEST,		// �f�[�^�]�������ݒ�
			nullptr,							// Clear�œK�l�B�g��Ȃ��̂�Nullptr
			IID_PPV_ARGS(&tex->buffer));			// �쐬����Resource�|�C���^�ւ̃|�C���^

	assert(SUCCEEDED(result));

	// SRV�q�[�v���쐬
	CreateSRV(*tex, tex->buffer.Get());

	// �T�u���\�[�X���쐬
	std::vector<D3D12_SUBRESOURCE_DATA> subResourcesDatas{};
	subResourcesDatas.resize(metadata.mipLevels);

	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < subResourcesDatas.size(); i++) {
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^�]��
		subResourcesDatas[i].pData = img->pixels;
		subResourcesDatas[i].RowPitch = (UINT)img->rowPitch;
		subResourcesDatas[i].SlicePitch = (UINT)img->slicePitch;

		assert(SUCCEEDED(result));
	}

	uint64_t uploadSize = GetRequiredIntermediateSize(tex->buffer.Get(), 0, (UINT)metadata.mipLevels);

	// �q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp1{};
	textureHeapProp1.Type = D3D12_HEAP_TYPE_UPLOAD;
	CD3DX12_RESOURCE_DESC textureResourceDesc1 =
		CD3DX12_RESOURCE_DESC::Buffer(uploadSize);

	Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;

	// �e�N�X�`���o�b�t�@�̐���
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
		tex->buffer.Get(),
		uploadBuffer.Get(),
		0,
		0,
		(UINT)metadata.mipLevels,
		subResourcesDatas.data());

	// Texture�ւ̓]����͗��p�ł���悤�AD3D12_RESOUCE_STATE_COPY_DEST����D3D12_RESOURCE_STATE_GEMEROC_READ��ResourceState��ύX����
	D3D12_RESOURCE_BARRIER  barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = tex->buffer.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;

	dxcommon_->GetCommandList()->ResourceBarrier(1, &barrier);
	ExcuteComandList();

	return tex;
}

void TextureManager::StaticInitialize(DirectXCommon* dxcommon)
{
	dxcommon_ = dxcommon;

	// ���ʊm�F
	HRESULT result;

	// �f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	// �ݒ�����Ƃ�SRV�p�f�X�N���v�^�q�[�v�𐶐�
	result = dxcommon_->GetDevice()->CreateDescriptorHeap(&srvHeapDesc,
		IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));
}

void TextureManager::CreateSRV(Texture& texture, ID3D12Resource* buffer)
{
	// SRV�q�[�v�̐擪�n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();

	UINT descriptorSize = dxcommon_->GetDevice()->
		GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	srvCpuHandle.ptr += (SIZE_T)(descriptorSize * srvIncrementIndex);
	srvGpuHandle.ptr += (SIZE_T)(descriptorSize * srvIncrementIndex);

	texture.SetCpuHandle(srvCpuHandle);
	texture.SetGpuHandle(srvGpuHandle);

	// �V�F�[�_�[���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};	// srv�ݒ�\����
	srvDesc.Format = buffer->GetDesc().Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;	// 2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = textureResourceDesc.MipLevels;

	// �n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	dxcommon_->GetDevice()->
		CreateShaderResourceView(buffer, &srvDesc, srvCpuHandle);

	srvIncrementIndex++;
}

void TextureManager::ExcuteComandList()
{
	ID3D12GraphicsCommandList* iCommandList = dxcommon_->GetCommandList();

	iCommandList->Close();

	ID3D12CommandQueue* iCommandQueue = dxcommon_->GetCommandQueue();

	ID3D12CommandList* list[] = { iCommandList };
	iCommandQueue->ExecuteCommandLists(1, list);

	dxcommon_->PreIncrimentFenceValue();

	// �R�}���h�̎��s������҂�
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

	// �L���[���N���A
	result = dxcommon_->GetCommandAllocator()->Reset();
	assert(SUCCEEDED(result));
	// �ĂуR�}���h���X�g�𒙂߂鏀��
	result = iCommandList->Reset(dxcommon_->GetCommandAllocator(), nullptr);
	assert(SUCCEEDED(result));
}
