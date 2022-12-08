#pragma once
#include<DirectXTex.h>
#include"DirectXCommon.h"
#include<wrl.h>
#include<array>
#include<string>

class Texture
{
public:// �����o�֐�

	// �e�N�X�`���ǂݍ���
	static void LoadTexture(uint32_t index, const std::string& fileName);
	
	// �e�N�X�`���ň�x�K�v�ȏ�����
	static void StaticInitialize(DirectXCommon* dxcommon);
	
	// �e�N�X�`���ɕK�v�ȃR�}���h�ݒ�
	void SetTextureCommands(uint32_t index);

public:// �ÓI�����o�ϐ�
	// SRV�̍ő��
	static const size_t kMaxSRVCount = 2056;
	// DirextX�̊�Ղ��؂�Ă���
	static DirectXCommon* dxcommon_;


private:// �����o�ϐ�
	// �e�N�X�`���o�b�t�@
	static std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, kMaxSRVCount>textureBuffers_;
	// SRV�q�[�v
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap;
};

