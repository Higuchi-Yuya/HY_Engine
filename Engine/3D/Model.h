#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <d3d12.h>
#include <d3dx12.h>

class Model
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:// �T�u�N���X
	// ���_�f�[�^�\����
	struct VertexPosNormalUv
	{
		Vector3 pos; // xyz���W
		Vector3 normal; // �@���x�N�g��
		Vector2 uv;  // uv���W
	};

	// �}�e���A��
	struct Material
	{
		std::string name;// �}�e���A����
		Vector3 ambient; // �A���r�G���g�e���x
		Vector3 diffuse; // �f�B�t���[�Y�e���x
		Vector3 specular; // �X�y�L�����[�e���x
		float alpha; // �A���t�@
		std::string textureFilename; // �e�N�X�`���t�@�C����
		// �R���X�g���N�^
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

	// �萔�o�b�t�@�p�f�[�^�\����B1
	struct ConstBufferDataB1
	{
		Vector3 ambient; // �A���r�G���g�W��
		float pad1; // �p�f�B���O
		Vector3 diffuse; // �f�B�t���[�Y�W��
		float pad2; // �p�f�B���O
		Vector3 specular; // �X�y�L�����[�W��
		float alpha; // �A���t�@
	};

public:// �����o�֐�
	Model();
	~Model();

	// OBJ�t�@�C������3D���f����ǂݍ���
	static Model* LoadFromOBJ(const std::string& modelname);

	/// <summary>
	/// �e�N�X�`���ǂݍ���
	/// </summary>
	void LoadTexture(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// �}�e���A���ǂݍ���
	/// </summary>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	// �`��
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndexMaterial);


	// �f�o�C�X�̃Z�b�^�[
	static void SetDevice(ID3D12Device* device) { Model::device = device; }

private:
	// �ÓI�Ȋ֐����ł̓����o�ϐ����Ăяo���Ȃ����߃v���C�x�[�g�����o�֐��̃p�b�P�[�W�������
	// LoadFromOBJ�̒��g
	void LoadFromOBJInternal(const std::string& modelname);

	/// <summary>
	/// �f�X�N���v�^�q�[�v�̏�����
	/// </summary>
	void InitializeDescriptorHeap();

	/// <summary>
	/// �e��o�b�t�@����
	/// </summary>
	void CreateBuffers();

private:// �����o�ϐ�

	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices;

	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;

	// �}�e���A��
	Material material;

	// �f�o�C�X�i�؂�Ă���j
	static ID3D12Device* device;

	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;

	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;

	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texbuff;

	// �f�X�N���v�^�T�C�Y
	UINT descriptorHandleIncrementSize;

	// �f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeap;

	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDescHandleSRV;

	// �V�F�[�_���\�[�X�r���[�̃n���h��(CPU)
	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuDescHandleSRV;

	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView;

	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView;

	// �}�e���A���p�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuffB1; 
};