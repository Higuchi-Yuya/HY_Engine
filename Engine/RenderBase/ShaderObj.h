#pragma once
#include<wrl.h>
#include<d3dx12.h>
#include<d3dcompiler.h>
#include<cassert>
#include<filesystem>

class ShaderObj
{
public:// �T�u�N���X
	//�^�C�v
	enum class ShaderType
	{
		VS,//�o�[�e�N�X
		PS,//�s�N�Z��
		DS,//�h���C��
		HS,//�n��
		GS,//�W�I���g��
		CS,//�R���s���[�g
	};

public:// �����o�֐�

	/// <summary>
	/// �V�F�[�_�I�u�W�F�N�g�𐶐�
	/// </summary>
	void Create(const std::string& fileName_, const std::string& entryPoint_, const std::string& target_, const ShaderType& type_);

	/// <summary>
	/// �V�F�[�_�I�u�W�F�N�g���擾
	/// </summary>
	D3D12_SHADER_BYTECODE* GetShader() {return &shaderBytecode;}

	/// <summary>
	/// �^�C�v
	/// </summary>
	const ShaderType& GetType() { return type; }

private:// �����o�ϐ�
	//�V�F�[�_�I�u�W�F�N�g
	Microsoft::WRL::ComPtr<ID3DBlob> blob;

	//�V�F�[�_�o�C�g�R�[�h
	D3D12_SHADER_BYTECODE shaderBytecode{};

	//�^�C�v
	ShaderType type = ShaderType::VS;

};

