#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <d3d12.h>
#include<wrl.h>
class Light
{
private:
	// namespace�̏ȗ�
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public:// �T�u�N���X
	struct ConstBufferDataLight
	{
		Vector3 lightv;// ���C�g�ւ̕�����\���x�N�g��
		float pad2; // �p�f�B���O
		Vector3 lightcolor;// ���C�g�̐F
	};

private:// �ÓI�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* device;

public:// �ÓI�����o�֐�
	// �ÓI������
	static void StaticInititalize(ID3D12Device* device);

public:// �����o�֐�

	// �C���X�^���X����
	static Light* Create();

	// ������
	void Initialize();

	// �X�V����
	void Update();

	// �`��
	void Draw(ID3D12GraphicsCommandList* cmdList);


	// �Z�b�^�[
	// ���C�g�������Z�b�g
	void SetLightDir(const Vector3& lightdir);

	// ���C�g�̐F���Z�b�g
	void SetLightColor(const Vector3& lightcolor);

private:// �v���C�x�[�g�����o�֐�

	/// <summary>
	/// �萔�o�b�t�@����
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// �}�b�s���O����
	/// </summary>
	void Map();

	/// <summary>
	/// �s��̓]��
	/// </summary>
	void TranferConstBuffer();
private:// �����o�ϐ�
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;

	// ���C�g���������i�P�ʃx�N�g���j
	Vector3 lightdir = { 1,0,1 };
	// ���C�g�̐F
	Vector3 lightcolor = { 1.0f,1.0f,1.0f };
	// �_�[�e�B�t���O
	bool dirty = false;


};

