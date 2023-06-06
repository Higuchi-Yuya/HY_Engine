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
public:// �����o�֐�

	// �e�N�X�`���ǂݍ���
	static Texture Load2DTexture(const std::string& fileName = "NULL");

	static uint32_t LoadTexture(const std::string& fileName = "NULL");

	static uint32_t LoadTexture(const wchar_t* fileName);

	// �e�N�X�`���ň�x�K�v�ȏ�����
	static void StaticInitialize(DirectXCommon* dxcommon);

	static void CreateSRV(Texture& texture, ID3D12Resource* buffer);

public:// �ÓI�����o�ϐ�
	static UINT srvIncrementIndex;

	// SRV�̍ő��
	static const size_t kMaxSRVCount = 2056;

	// �f�t�H���g�e�N�X�`��2D�i�[�f�B���N�g��
	static std::string kDefault2DTextureDirectoryPath;

	// �f�t�H���g�e�N�X�`���i�[�f�B���N�g��
	static std::string kDefaultTextureDirectoryPath;

	// DirextX�̊�Ղ��؂�Ă���
	static DirectXCommon* dxcommon_;

	// SRV�q�[�v
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;

	// �e�N�X�`���o�b�t�@
	static std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxSRVCount>textureBuffers_;

	//// �e�N�X�`���S��
	//static std::map<std::string, std::unique_ptr<Texture>> textureMap;

private:// �v���C�x�[�g�֐�
	//[[nodiscard]]
	ID3D12Resource* UpLoadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

	// 
	ID3D12Resource* CreateBufferResource(ID3D12Device* device, uint64_t intermediateSize);

	// �R�}���h�����ς񂾂��Ƃɉ������֐�
	static void ExcuteComandList();

private:// �����o�ϐ�
	// �e�N�X�`�����\�[�X�f�X�N
	static D3D12_RESOURCE_DESC textureResourceDesc;


};

