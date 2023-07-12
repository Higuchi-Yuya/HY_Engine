#pragma once
#include"Vector3.h"
#include "Vector4.h"
#include "TextureManager.h"
#include<d3d12.h>
#include<wrl.h>

class Dissolve
{
public:// �T�u�N���X
//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferDataDissolve {
		Vector4 dissolveColor; // �f�B�]���u�̏����鎞�̐F
		bool isActiveDissolve; // �t�H�O���|���邩�ǂ����̃t���O
		float dissolvePower;	  // �f�B�]���u�̏�����Ƃ��̐F�̓x��
		float dissolveTime;    // �f�B�]���u�̏�����܂ł̎��Ԃ̊���
		float dissolveSmoothMin;// �f�B�]���u�X���[�X�̍ŏ��l
	};
public:// �����o�֐�
	~Dissolve();

	// �ÓI������
	static void StaticInitialize(ID3D12Device* device);

	static Dissolve* Create();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �s����X�V����
	/// </summary>
	void UpdateMatrix();

	// �`��
	void Draw(ID3D12GraphicsCommandList* cmdList, uint32_t dissolveRootIndex);

	// �o�b�t�@�̃Q�b�^�[
	ID3D12Resource* GetBuff() { return constBuff_.Get(); }

	// �e�N�X�`���̃Z�b�g
	void SetDissolveTexture(Texture* dissolveTex) { dissolveTex_ = dissolveTex; }

private:// �v���C�x�[�g�֐�
	/// <summary>
	/// �萔�o�b�t�@����
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// �}�b�s���O����
	/// </summary>
	void Map();

public:// �O������ύX�\�Ȓl
	bool isActiveDissolve_ = false;
	float dissolvePower_ = 10.0f;
	float dissolveTime_ = 0.0f;
	float dissolveSmoothMin_ = -0.5;
	Vector4 dissolveColor_ = { 1.0f,1.0f,1.0f,1.0f };

private:// �����o�ϐ�
	// �f�o�C�X�i�؂�Ă���j
	static ID3D12Device* sDevice_;

	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;

	// �}�b�s���O�ς݃A�h���X
	ConstBufferDataDissolve* constMap_ = nullptr;

	// �f�t�H���g�̃f�B�]���u�e�N�X�`���̃��[�g�p�X
	static const std::string sDissolveTexBasePass_;

	// �f�B�]���u�̃e�N�X�`��
	Texture* dissolveTex_;
};

