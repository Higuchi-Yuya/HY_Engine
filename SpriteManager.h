#pragma once
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

class SpriteManager
{
public:// �����o�֐�
	// ������
	void Initialize(DirectXCommon* dxcommon);

	// �`��
	void Draw();

public:
	static DirectXCommon* dxcommon_;
private:

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;		//�p�C�v���C���X�e�[�g
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;		//���[�g�V�O�l�`��
	// ���ʊm�F
	HRESULT result;
};

