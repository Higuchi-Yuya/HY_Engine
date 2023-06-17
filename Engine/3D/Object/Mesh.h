#pragma once
#include <vector>
#include <unordered_map>
#include "Material.h"

// �m�[�h
struct Node
{
	//���O
	std::string name;
	//���[�J���ό`�s��
	Matrix4 transform;
	//�O���[�o���ό`�s��
	Matrix4 globalTransform;
	//�e�m�[�h
	Node* parent = nullptr;
	//�q�m�[�h
	std::vector<Node*>childrens;

};

class Mesh
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:// �T�u�N���X
	friend class FbxLoader;
	friend class FbxModel;
	static const int sMAX_BONE_INDICES = 4;

	// ��
	struct Bone
	{
		//���O
		std::string name;

		Matrix4 matrix;
		Matrix4 animationMatrix;
		Matrix4 offsetMatirx;

		UINT index;

	};

	// ���_�f�[�^�\����
	struct VertexPosNormalUv
	{
		Vector3 pos; // xyz���W
		Vector3 normal; // �@���x�N�g��
		Vector2 uv;  // uv���W

		uint32_t boneIndex[sMAX_BONE_INDICES];
		float boneWeight[sMAX_BONE_INDICES];
	};



public:
	// �f�o�C�X�̃Z�b�^�[
	static void SetDevice(ID3D12Device* device);

	// �`��
	void Draw(ID3D12GraphicsCommandList* cmdList);

	// �G�b�W�������f�[�^�̒ǉ�
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);

	// ���������ꂽ���_�@���̌v�Z
	void CalculateSmoothedVertexNormals();

	/// <summary>
	/// ���O���擾
	/// </summary>
	/// <returns>���O</returns>
	const std::string& GetName() { return name_; }

	/// <summary>
	/// ���O���Z�b�g
	/// </summary>
	/// <param name="name">���O</param>
	void SetName(const std::string& name);

	/// <summary>
	/// ���_�f�[�^�̒ǉ�
	/// </summary>
	/// <param name="vertex">���_�f�[�^</param>
	void AddVertex(const VertexPosNormalUv& vertex);

	/// <summary>
	/// ���_�C���f�b�N�X�̒ǉ�
	/// </summary>
	/// <param name="index">�C���f�b�N�X</param>
	void AddIndex(unsigned short index);

	// ���_�f�[�^�̐����擾
	inline size_t GetVertexCount() { return vertices_.size(); }

	/// <summary>
	/// �}�e���A���̎擾
	/// </summary>
	/// <returns>�}�e���A��</returns>
	Material* GetMaterial() { return material_; }

	/// <summary>
	/// �}�e���A���̊��蓖��
	/// </summary>
	/// <param name="material">�}�e���A��</param>
	void SetMaterial(Material* material);

	/// <summary>
	/// �o�b�t�@�̐���
	/// </summary>
	void CreateBuffers();

	/// <summary>
	/// ���_�o�b�t�@�擾
	/// </summary>
	/// <returns>���_�o�b�t�@</returns>
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView_; }

	/// <summary>
	/// �C���f�b�N�X�o�b�t�@�擾
	/// </summary>
	/// <returns>�C���f�b�N�X�o�b�t�@</returns>
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView_; }

	/// <summary>
	/// ���_�z����擾
	/// </summary>
	/// <returns>���_�z��</returns>
	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices_; }

	/// <summary>
	/// �C���f�b�N�X�z����擾
	/// </summary>
	/// <returns>�C���f�b�N�X�z��</returns>
	inline const std::vector<unsigned short>& GetIndices() { return indices_; }

private:

	// �f�o�C�X�i�؂�Ă���j
	static ID3D12Device* sDevice_;

	// ���O
	std::string name_;

	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices_;

	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices_;

	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff_;

	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff_;

	//�{�[���o�b�t�@
	ComPtr<ID3D12Resource> BoneBuff_;

	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};

	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};

	// ���_�@���X���[�W���O�p�f�[�^
	std::unordered_map<unsigned short, std::vector<unsigned short>>smoothData_;

	// �}�e���A��
	Material* material_ = nullptr;

	// �{�[��
	std::unordered_map<std::string, Bone*> bones;

	std::vector<Bone> vecBones;

	// �m�[�h
	Node* node = nullptr;

	// �}�b�s���O�ς݃A�h���X
	Bone* constMap = nullptr;
};

