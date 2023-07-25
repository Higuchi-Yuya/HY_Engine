#pragma once
#include "DirectXCommon.h"
#include "Texture.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <TextureManager.h>
#include "ShaderObj.h"

class SpriteManager
{
public:// �T�u�N���X
	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		Vector4 color;//�F(RGBA)
		Matrix4 mat;// 3D�ϊ��s��
	};

	// ���_�f�[�^�\����
	struct Vertex
	{
		Vector3 pos; // xyz���W
		Vector2 uv;  // uv���W
	};

public:// �����o�֐�
	// ������
	void Initialize(DirectXCommon* dxcommon);

	// �`��O�ݒ�
	void PreDraw();

	// �`���ݒ�
	void PostDraw();

	// �e�N�X�`���ɕK�v�ȃR�}���h�ݒ�
	void SetTextureCommands(Texture* index);

	// �p�C�v���C���̃A�N�Z�X
	ID3D12PipelineState *GetPipelineState() { return pipelineState_.Get(); }

	// ���[�g�V�O�l�`���̃A�N�Z�X
	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }

public:
	static DirectXCommon* sDxcommon_;
private:
	
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;		//�p�C�v���C���X�e�[�g
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;		//���[�g�V�O�l�`��

	ShaderObj vsShader_;// ���_�V�F�[�_�[
	ShaderObj psShader_;// �s�N�Z���V�F�[�_�[

	// ���ʊm�F
	HRESULT result = 0;
};
