#pragma once
#include "DirectionalLight.h"
#include <d3d12.h>
#include <wrl.h>
class LightGroup
{
private:
	// namespace�̏ȗ�
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

public:// �萔
	// ���s�����̐�
	static const int kDirLightNum = 3;
	// �_�����̐�
	static const int kPointLightNum = 3;
	// �X�|�b�g���C�g�̐�
	static const int kSpotLightNum = 3;
	// �ۉe�̐�
	static const int kCircleShadowNum = 1;

public:// �T�u�N���X
	struct ConstBufferData
	{
		// �����̐F
		Vector3 ambientColor;
		float pad1;
		// ���s�����p
		DirectionalLight::ConstBufferData dirLights[kDirLightNum];
	};

private:// �ÓI�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* device;

public:// �ÓI�����o�֐�
	// �ÓI������
	static void StaticInititalize(ID3D12Device* device);

public:// �����o�֐�

	// �C���X�^���X����
	static LightGroup* Create();

	// ������
	void Initialize();

	// �X�V����
	void Update();

	// �`��
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �����̃��C�g�F���Z�b�g
	/// </summary>
	/// <param name="color">���C�g�F</param>
	void SetAmbientColor(const Vector3& color);

	/// <summary>
	/// ���s�����̗L���t���O���Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="active">�L���t���O</param>
	void SetDirLightActive(int index, bool active);

	/// <summary>
	/// ���s�����̃��C�g�������Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="lightdir">���C�g����</param>
	void SetDirLightDir(int index, const Vector3& lightdir);

	/// <summary>
	/// ���s�����̃��C�g�F���Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="lightcolor">���C�g�F</param>
	void SetDirLightColor(int index, const Vector3& lightcolor);

private:// �v���C�x�[�g�����o�֐�

	/// <summary>
	/// �萔�o�b�t�@����
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// �萔�o�b�t�@�]��
	/// </summary>
	void TransferConstBuffer();

	/// <summary>
	/// �W���̃��C�g�ݒ�
	/// </summary>
	void DefaultLightSetting();

private:// �����o�ϐ�
	// �萔�o�b�t�@
	ComPtr<ID3D12Resource> constBuff;
	// �萔�o�b�t�@�̃}�b�v
	ConstBufferData* constMap = nullptr;

	// �����̐F
	Vector3 ambientColor_ = { 1,1,1 };

	// ���s�����̔z��
	DirectionalLight dirLights_[kDirLightNum];


	// �_�[�e�B�t���O
	bool dirty = false;

};

