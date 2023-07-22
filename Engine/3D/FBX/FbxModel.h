#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>

#include "Quaternion.h"
#include "Texture.h"
#include <Mesh.h>
#include "Material.h"
#include <LightGroup.h>
#include "WorldTransform.h"
#include <ViewProjection.h>
#include <assimp/anim.h>
#include "ShaderObj.h"

class FbxModel
{
public:
	// �t�����h�N���X
	friend class FbxLoader;

public://�萔
	static const int MAX_BONE_INDICES = 4;


public: // �T�u�N���X
	// ���_�f�[�^�\����
	struct VertexPosNormalUvFbxSkin
	{
		DirectX::XMFLOAT3 pos; // xyz���W
		DirectX::XMFLOAT3 normal; // �@���x�N�g��
		DirectX::XMFLOAT2 uv;  // uv���W

		uint32_t boneIndex[MAX_BONE_INDICES];
		float boneWeight[MAX_BONE_INDICES];
	};
	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferPolygonExplosion
	{
		float Destruction = 0.0f;
		float ScaleFactor = 1.0f;
		float RotationFactor = 0.0f;
		float PositionFactor = 0.0f;
	};

private:
	// Microsoft::WRL::���ȗ�
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


public:

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		Vector4 color;	// �F (RGBA)
		Matrix4 mat;	// �R�c�ϊ��s��
	};

	//1���b�V���Ɏ��Ă�{�[���̍ő��
	static const int MAX_BONES = 128;

	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferDataSkin {
		Matrix4 bones[MAX_BONES];
	};

	//�A�j���[�V�������Ȃ��ꍇ
	struct ConstBufferDataInitialMatrix {
		Matrix4 InitialMatrix;
	};


	//�萔�o�b�t�@(�X�L��)
	static Microsoft::WRL::ComPtr<ID3D12Resource> constBuffSkin_;
	//�萔�o�b�t�@(�A�j���[�V�����Ȃ�)
	static Microsoft::WRL::ComPtr<ID3D12Resource> constBuffNothing_;

private:
	static const std::string kBaseDirectory_;
	static const std::string kDefaultModelName_;

private: // �ÓI�����o�ϐ�
	// �f�X�N���v�^�T�C�Y
	static uint32_t sDescriptorHandleIncrementSize_;
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList* sCommandList_;
	// ���[�g�V�O�l�`��
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;
	// ���C�g
	static std::unique_ptr<LightGroup> lightGroup_;
	// �؂�Ă���f�o�C�X
	static ID3D12Device* device_;

	static ShaderObj* sVsShader_;// ���_�V�F�[�_�[
	static ShaderObj* sPsShader_;// �s�N�Z���V�F�[�_�[
	static ShaderObj* sGsShader_;// �W�I���g���V�F�[�_�[

public: // �ÓI�����o�֐�


	// �ÓI������
	static void StaticInitialize();

	// �ÓI�I������
	static void StaticFainalize();

	// �O���t�B�b�N�X�p�C�v���C���̏�����
	static void InitializeGraphicsPipeline();

	// 3D���f������
	static FbxModel* Create();

	// OBJ�t�@�C�����烁�b�V������
	static FbxModel* CreateFromFbx(const std::string& modelname, bool smoothing = false);

	// �؂�Ă���f�o�C�X���Z�b�g
	static void SetDevice(ID3D12Device* device) { device_ = device; }

	// �`��O����
	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	// �`��㏈��
	static void PostDraw();

public: // �����o�֐�
	// �f�X�g���N�^
	~FbxModel();

	// ������
	void Initialize();

	void FbxUpdate(float frem);


	// �`��
	//void Draw(
	//	const WorldTransform& worldTransform, const ViewProjection& viewProjection);

	void Draw(WorldTransform* worldTransform, ViewProjection* viewProjection);

	void Draw(WorldTransform* worldTransform, ViewProjection* viewProjection, Texture textureHadle);


	void ModelAnimation(float frame, aiAnimation* Animation, int BoneNum);

	void ReadNodeHeirarchy(Mesh* mesh, aiAnimation* Animation, FLOAT AnimationTime, Node* pNode, Matrix4& mxIdentity);

	aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string& strNodeName);

	void CalcInterpolatedScaling(Vector3& mxOut, float AnimationTime, const aiNodeAnim* pNodeAnim);

	bool FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, uint32_t& nScalingIndex);

	void CalcInterpolatedRotation(Vector4& mxOut, float AnimationTime, const aiNodeAnim* pNodeAnim);

	bool FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, uint32_t& nRotationIndex);

	void CalcInterpolatedPosition(Vector3& mxOut, float AnimationTime, const aiNodeAnim* pNodeAnim);

	bool FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, uint32_t& nPosIndex);

	const ConstBufferPolygonExplosion GetPolygonExplosion() { return *constMap_; }

	const void SetPolygonExplosion(ConstBufferPolygonExplosion polygonExplosion) { *constMap_ = polygonExplosion; }

	// ���b�V���R���e�i���擾
	inline const std::vector<Mesh*>& GetMeshes() { return meshes_; }

	Matrix4 GetLeftBonePos();
	Matrix4 GetRightBonePos();

	void SetTextureHandle(Texture textureHandle) { modelTextureHandle_ = textureHandle; }

	

private:

	// �m�[�h�z��
	std::vector<Node> nodes_;

	Matrix4 globalInverseTransform_;

	// ���O
	std::string name_;
	// ���b�V���R���e�i
	std::vector<Mesh*> meshes_;
	// �}�e���A���R���e�i
	std::unordered_map<std::string, Material*> materials_;
	// �f�t�H���g�}�e���A��
	Material* defaultMaterial_ = nullptr;

	Texture modelTextureHandle_;

	Matrix4 matrixL_;
	Matrix4 matrixR_;

	WorldTransform naosi_;

	//�萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff_;

	ConstBufferPolygonExplosion* constMap_;

};



