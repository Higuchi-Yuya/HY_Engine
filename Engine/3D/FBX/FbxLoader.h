#pragma once

#include "FbxModel.h"
#include "Texture.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <string>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<assimp/cimport.h>

class FbxLoader
{
private: // �G�C���A�X
	// std::���ȗ�
	using string = std::string;

public: // �萔
	// ���f���i�[���[�g�p�X
	static const string sBaseDirectory;

public:// �����o�֐�

	/// <summary>
	/// �V���O���g���C���X�^���X�̎擾
	/// </summary>
	/// <returns>�C���X�^���X</returns>
	static FbxLoader* GetInstance();

	/// <summary>
	/// ������
	/// </summary>
	/// <param name="device">D3D12�f�o�C�X</param>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// ��n��
	/// </summary>
	void Finalize();

	/// <summary>
	/// �t�@�C������FBX���f���Ǎ�
	/// </summary>
	/// <param name="modelName">���f����</param>
	FbxModel* LoadModelFromFile(const string& modelName);

	void ParseSkin(FbxModel* model, aiMesh* fbxMesh);

	void GetNodeNum(const aiNode* node, UINT32& num);

private:// �v���C�x�[�g�֐�
	
	/// <summary>
	/// �ċA�I�Ƀm�[�h�\�������
	/// </summary>
	/// <param name="model">�ǂݍ��ݐ惂�f���I�u�W�F�N�g</param>
	/// <param name="fbxNode">��͑Ώۂ̃m�[�h</param>
	/// <param name="parent">�e�m�[�h</param>
	void ParseNodeRecursive(FbxModel* model, aiNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// ���b�V���ǂݎ��
	/// </summary>
	/// <param name="model">�ǂݍ��ݐ惂�f���I�u�W�F�N�g</param>
	/// <param name="fbxNode">��͑Ώۂ̃m�[�h</param>
	void ParseMesh(FbxModel* model, aiMesh* fbxNode);
	
	// ���_���W�ǂݎ��
	void ParseMeshVertices(FbxModel* model, aiMesh* fbxMesh);
	
	// �ʏ��ǂݎ��
	void ParseMeshFaces(FbxModel* model, aiMesh* fbxMesh);
	
	// �}�e���A���ǂݎ��
	void ParseMaterial(FbxModel* model, aiMesh* fbxMesh, aiMaterial* aimaterial);

	// �f�B���N�g�����܂񂾃t�@�C���p�X����t�@�C�����𒊏o����
	std::string ExtractFileName(const std::string& path);

	Texture LoadMatrixerialTextures(aiMaterial* cmatrix, aiTextureType type, std::string typeName, const aiScene* scene_, const std::string& modelName);

private:// �����o�ϐ�

	// private�ȃR���X�g���N�^�i�V���O���g���p�^�[���j
	FbxLoader() = default;
	// private�ȃf�X�g���N�^�i�V���O���g���p�^�[���j
	~FbxLoader() = default;
	// �R�s�[�R���X�g���N�^���֎~�i�V���O���g���p�^�[���j
	FbxLoader(const FbxLoader & obj) = delete;
	// �R�s�[������Z�q���֎~�i�V���O���g���p�^�[���j
	void operator=(const FbxLoader & obj) = delete;

	// �C���X�^���X�p�̃��[�_�[
	static FbxLoader* sFbxLoader_;

	// D3D12�f�o�C�X�i�؂�ė�����̂̓��ꕨ�j
	ID3D12Device* device_ = nullptr;

	// �ݒ�̏�Ԃ�\���t���O
	const uint32_t flag_ =
		aiProcess_Triangulate | //�O�p�ʉ�
		aiProcess_CalcTangentSpace | //�ڐ��x�N�g������
		aiProcess_GenSmoothNormals | //�X���[�W���O�x�N�g������
		aiProcess_GenUVCoords | //��}�b�s���O��K�؂�UV���W�ɕϊ�
		aiProcess_RemoveRedundantMaterials | //�璷�ȃ}�e���A�����폜
		aiProcess_OptimizeMeshes | //���b�V�������œK��
		aiProcess_MakeLeftHanded | //�m�[�h��������W�n��
		aiProcess_GenBoundingBoxes | //AABB�𐶐�
		aiProcess_JoinIdenticalVertices |//�C���f�b�N�X�𐶐�
		aiProcess_LimitBoneWeights;//�e���_���e�����󂯂�{�[����4�ɐ���

	// �V�[��
	const aiScene* mScene_;

	// �e�N�X�`��
	Texture textureHandle_;
};

