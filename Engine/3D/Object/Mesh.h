#pragma once
#include <vector>
#include <unordered_map>
#include "Material.h"
class Mesh
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
	void SetName(const std::string& name_);

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
	inline size_t GetVertexCount() { return vertices.size(); }

	/// <summary>
	/// �}�e���A���̎擾
	/// </summary>
	/// <returns>�}�e���A��</returns>
	Material* GetMaterial() { return material; }

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
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView; }

	/// <summary>
	/// �C���f�b�N�X�o�b�t�@�擾
	/// </summary>
	/// <returns>�C���f�b�N�X�o�b�t�@</returns>
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView; }

	/// <summary>
	/// ���_�z����擾
	/// </summary>
	/// <returns>���_�z��</returns>
	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices; }

	/// <summary>
	/// �C���f�b�N�X�z����擾
	/// </summary>
	/// <returns>�C���f�b�N�X�z��</returns>
	inline const std::vector<unsigned short>& GetIndices() { return indices; }

private:

	// �f�o�C�X�i�؂�Ă���j
	static ID3D12Device* device;

	// ���O
	std::string name_;

	// ���_�f�[�^�z��
	std::vector<VertexPosNormalUv> vertices;

	// ���_�C���f�b�N�X�z��
	std::vector<unsigned short> indices;

	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;

	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff;

	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView = {};

	// �C���f�b�N�X�o�b�t�@�r���[
	D3D12_INDEX_BUFFER_VIEW ibView = {};

	// ���_�@���X���[�W���O�p�f�[�^
	std::unordered_map<unsigned short, std::vector<unsigned short>>smoothData;

	// �}�e���A��
	Material* material = nullptr;
};

