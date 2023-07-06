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

	// �t���[�Ƀp�X���w�肵�Ďg���p�̃e�N�X�`���ǂݍ���
	static Texture* LoadFreeTexture(const std::string& filePath = "NULL");

	// 2D�̃e�N�X�`���̓ǂݍ���
	static Texture Load2DTexture(const std::string& fileName = "NULL");
	static Texture* Load2DTextureP(const std::string fileName);

	// ���f���̃}�e���A���e�N�X�`���̓ǂݍ���
	static Texture LoadTexture(std::string fileName);
	static Texture* LoadTextureP(std::string fileName);

	// �e�N�X�`���ň�x�K�v�ȏ�����
	static void StaticInitialize(DirectXCommon* dxcommon);

	// �e�N�X�`���ŃX�^�e�B�b�N�̕ϐ������
	static void StaticFinalize();

public:// �ÓI�����o�ϐ�
	static uint32_t sSrvIncrementIndex;

	// SRV�̍ő��
	static const size_t sMaxSRVCount = 2056;

	// �f�t�H���g�e�N�X�`��2D�i�[�f�B���N�g��
	static std::string sDefault2DTextureDirectoryPath;

	// SRV�q�[�v
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> sSrvHeap;

private:// �v���C�x�[�g�֐�
	// �V�F�[�_���\�[�X�r���[�̍쐬
	static void CreateSRV(Texture& texture, ID3D12Resource* buffer);

	// �R�}���h�����ς񂾂��Ƃɉ������֐�
	static void ExcuteComandList();

private:// �����o�ϐ�
	// �e�N�X�`�����\�[�X�f�X�N
	static D3D12_RESOURCE_DESC sTextureResourceDesc;

	// DirextX�̊�Ղ��؂�Ă���
	static DirectXCommon* sDxcommon_;
};

