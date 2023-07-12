#pragma once
#include "Mesh.h"

class Model
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:// �����o�֐�
	Model();
	~Model();

	// OBJ�t�@�C������3D���f����ǂݍ���
	static Model* LoadFromOBJ(const std::string& modelname,bool smoothing = false);

	/// <summary>
	/// �}�e���A���ǂݍ���
	/// </summary>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	// �`��
	void Draw(ID3D12GraphicsCommandList* cmdList);


	// �f�o�C�X�̃Z�b�^�[
	static void SetDevice(ID3D12Device* device);

	/// <summary>
	/// ���b�V���R���e�i���擾
	/// </summary>
	/// <returns>���b�V���R���e�i</returns>
	inline const std::vector<Mesh*>& GetMeshes() { return meshes_; }
	

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

	// ���O
	std::string name_;

	// ���b�V���R���e�i
	std::vector<Mesh*> meshes_;

	// �}�e���A���R���e�i
	std::unordered_map<std::string, Material*> materials_;

	// �f�t�H���g�}�e���A��
	Material* defaultMaterial_ = nullptr;

	// �f�o�C�X�i�؂�Ă���j
	static ID3D12Device* sDevice_;




};