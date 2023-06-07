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

	// 2D�̃e�N�X�`���̓ǂݍ���
	static Texture Load2DTexture(const std::string& fileName = "NULL");
	static Texture* Load2DTextureP(const std::string fileName);

	// ���f���̃}�e���A���e�N�X�`���̓ǂݍ���
	static Texture LoadTexture(std::string fileName);

	static Texture* LoadTextureP(std::string fileName);

	// �e�N�X�`���ň�x�K�v�ȏ�����
	static void StaticInitialize(DirectXCommon* dxcommon);

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

private:// �v���C�x�[�g�֐�
	// �V�F�[�_���\�[�X�r���[�̍쐬
	static void CreateSRV(Texture& texture, ID3D12Resource* buffer);

	// �R�}���h�����ς񂾂��Ƃɉ������֐�
	static void ExcuteComandList();

private:// �����o�ϐ�
	// �e�N�X�`�����\�[�X�f�X�N
	static D3D12_RESOURCE_DESC textureResourceDesc;
};

