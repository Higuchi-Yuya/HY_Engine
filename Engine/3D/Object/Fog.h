#pragma once
#include"Vector3.h"
#include "Vector4.h"
#include<d3d12.h>
#include<wrl.h>

class Fog
{
public:// �T�u�N���X
//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferDataFog {
		Vector4 fogColor; // fog�̊|����F
		bool isActiveFog; // �t�H�O���|���邩�ǂ����̃t���O
		float nearFog;	  // fog�̊J�n�ʒu
		float farFog;	  // fog�̏I���ʒu
	};
public:// �����o�֐�
	// �ÓI������
	static void StaticInitialize(ID3D12Device* device);

	static Fog* Create();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �s����X�V����
	/// </summary>
	void UpdateMatrix();

	// �`��
	void Draw(ID3D12GraphicsCommandList* cmdList);

	// �o�b�t�@�̃Q�b�^�[
	ID3D12Resource* GetBuff() { return constBuff.Get(); }

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
	bool isActiveFog = false;
	float nearFog = 0.0f;
	float farFog = 50.0f;
	Vector4 fogColor = { 1.0f,1.0f,1.0f,1.0f };

private:// �����o�ϐ�
	// �f�o�C�X�i�؂�Ă���j
	static Microsoft::WRL::ComPtr<ID3D12Device> device_;

	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;

	// �}�b�s���O�ς݃A�h���X
	ConstBufferDataFog* constMap = nullptr;

};

