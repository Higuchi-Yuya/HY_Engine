#pragma once

#include <vector>
#include <unordered_map>

#include "Material.h"

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

	//// �}�e���A��
	//struct Material
	//{
	//	std::string name;// �}�e���A����
	//	Vector3 ambient; // �A���r�G���g�e���x
	//	Vector3 diffuse; // �f�B�t���[�Y�e���x
	//	Vector3 specular; // �X�y�L�����[�e���x
	//	float alpha; // �A���t�@
	//	std::string textureFilename; // �e�N�X�`���t�@�C����
	//	// �R���X�g���N�^
	//	Material() {
	//		ambient = { 0.3f,0.3f,0.3f };
	//		diffuse = { 0.0f,0.0f,0.0f };
	//		specular = { 0.0f,0.0f,0.0f };
	//		alpha = 1.0f;
	//	}
	//};



public:// �����o�֐�
	Model();
	~Model();

	// OBJ�t�@�C������3D���f����ǂݍ���
	static Model* LoadFromOBJ(const std::string& modelname,bool smoothing = false);

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
	static void SetDevice(ID3D12Device* device);

	// �G�b�W�������f�[�^�̒ǉ�
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);

	// ���������ꂽ���_�@���̌v�Z
	void CalculateSmoothedVertexNormals();

	// ���_�f�[�^�̐����擾
	inline size_t GetVertexCount() { return vertices.size(); }

private:
	// �ÓI�Ȋ֐����ł̓����o�ϐ����Ăяo���Ȃ����߃v���C�x�[�g�����o�֐��̃p�b�P�[�W�������
	// LoadFromOBJ�̒��g
	void LoadFromOBJInternal(const std::string& modelname, bool smoothing = false);

	/// <summary>
	/// �e��o�b�t�@����
	/// </summary>
	void CreateBuffers();

	/// <summary>
	/// �}�e���A���̓o�^
	/// </summary>
	/// <param name="material"></param>
	void AddMaterial(Material* material);

private:// �����o�ϐ�

	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices;

	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;

	//// �}�e���A��
	//Material material;

	// �}�e���A���R���e�i
	std::unordered_map<std::string, Material*> materials;

	// �f�o�C�X�i�؂�Ă���j
	static ID3D12Device* device;

	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;

	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;

	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView;

	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView;



	// ���_�@���X���[�W���O�p�f�[�^
	std::unordered_map<unsigned short, std::vector<unsigned short>>smoothData;


};