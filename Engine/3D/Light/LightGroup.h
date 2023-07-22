#pragma once
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "CircleShadow.h"
#include <d3d12.h>
#include <wrl.h>
class LightGroup
{
public:// �萔
	// ���s�����̐�
	static const int sDirLightNum = 3;
	// �_�����̐�
	static const int sPointLightNum = 3;
	// �X�|�b�g���C�g�̐�
	static const int sSpotLightNum = 3;
	// �ۉe�̐�
	static const int sCircleShadowNum = 1;

public:// �T�u�N���X
	struct ConstBufferData
	{
		// �����̐F
		Vector3 ambientColor;
		float pad1;
		Vector3 diffuseColor;// �f�B�t���[�Y�J���[
		float pad2;
		Vector3 specularColor; // �X�y�L�����[�J���[
		float pad3; // �p�f�B���O
		// ���s�����p
		DirectionalLight::ConstBufferData dirLights[sDirLightNum];
		// �_�����p
		PointLight::ConstBufferData pointLights[sPointLightNum];
		// �X�|�b�g���C�g�p
		SpotLight::ConstBufferData spotLights[sSpotLightNum];
		// �ۉe�p
		CircleShadow::ConstBufferData circleShadows[sCircleShadowNum];
	};

private:// �ÓI�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* sDevice;

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
	void Draw(ID3D12GraphicsCommandList* cmdList, uint32_t rootParameterIndex);

	/// <summary>
	/// �����̃��C�g�F���Z�b�g
	/// </summary>
	/// <param name="color">���C�g�F</param>
	void SetAmbientColor(const Vector3& color);

	/// <summary>
	/// �f�B�t���[�Y�̃��C�g�F���Z�b�g
	/// </summary>
	/// <param name="color">���C�g�F</param>
	void SetDiffuseColor(const Vector3& color);

	/// <summary>
	/// �X�y�L�����[�̃��C�g�F���Z�b�g
	/// </summary>
	/// <param name="color">���C�g�F</param>
	void SetSpecularColor(const Vector3& color);

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

	/// <summary>
	/// �_�����̗L���t���O���Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="active">�L���t���O</param>
	void SetPointLightActive(int index, bool active);

	/// <summary>
	/// �_�����̃��C�g���W���Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="lightpos">���C�g���W</param>
	void SetPointLightPos(int index, const Vector3& lightpos);

	/// <summary>
	/// �_�����̃��C�g�F���Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="lightcolor">���C�g�F</param>
	void SetPointLightColor(int index, const Vector3& lightcolor);

	/// <summary>
	/// �_�����̃��C�g���������W�����Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="lightatten">���C�g���������W��</param>
	void SetPointLightAtten(int index, const Vector3& lightAtten);

	/// <summary>
	/// �X�|�b�g���C�g�̗L���t���O���Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="active">�L���t���O</param>
	void SetSpotLightActive(int index, bool active);

	/// <summary>
	/// �X�|�b�g���C�g�̃��C�g�������Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="lightdir">���C�g����</param>
	void SetSpotLightDir(int index, const Vector3& lightdir);

	/// <summary>
	/// �X�|�b�g���C�g�̃��C�g���W���Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="lightpos">���C�g���W</param>
	void SetSpotLightPos(int index, const Vector3& lightpos);

	/// <summary>
	/// �X�|�b�g���C�g�̃��C�g�F���Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="lightcolor">���C�g�F</param>
	void SetSpotLightColor(int index, const Vector3& lightcolor);

	/// <summary>
	/// �X�|�b�g���C�g�̃��C�g���������W�����Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="lightatten">���C�g���������W��</param>
	void SetSpotLightAtten(int index, const Vector3& lightAtten);

	/// <summary>
	/// �X�|�b�g���C�g�̃��C�g�����p�x���Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="lightFactorAngle">x:�����J�n�p�x y:�����I���p�x</param>
	void SetSpotLightFactorAngle(int index, const Vector2& lightFactorAngle);

	/// <summary>
	/// �ۉe�̗L���t���O���Z�b�g
	/// </summary>
	/// <param name="index">���C�g�ԍ�</param>
	/// <param name="active">�L���t���O</param>
	void SetCircleShadowActive(int index, bool active);

	/// <summary>
	/// �ۉe�̃L���X�^�[���W���Z�b�g
	/// </summary>
	/// <param name="index">�ԍ�</param>
	/// <param name="lightpos">�L���X�^�[���W</param>
	void SetCircleShadowCasterPos(int index, const Vector3& casterPos);

	/// <summary>
	/// �ۉe�̕������Z�b�g
	/// </summary>
	/// <param name="index">�ԍ�</param>
	/// <param name="lightdir">����</param>
	void SetCircleShadowDir(int index, const Vector3& lightdir);

	/// <summary>
	/// �ۉe�̃L���X�^�[�ƃ��C�g�̋������Z�b�g
	/// </summary>
	/// <param name="index">�ԍ�</param>
	/// <param name="distanceCasterLight">�L���X�^�[�ƃ��C�g�̋���</param>
	void SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight);

	/// <summary>
	/// �ۉe�̋��������W�����Z�b�g
	/// </summary>
	/// <param name="index">�ԍ�</param>
	/// <param name="lightatten">���������W��</param>
	void SetCircleShadowAtten(int index, const Vector3& lightAtten);

	/// <summary>
	/// �ۉe�̌����p�x���Z�b�g
	/// </summary>
	/// <param name="index">�ԍ�</param>
	/// <param name="lightFactorAngle">x:�����J�n�p�x y:�����I���p�x</param>
	void SetCircleShadowFactorAngle(int index, const Vector2& lightFactorAngle);

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
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;

	// �萔�o�b�t�@�̃}�b�v
	ConstBufferData* constMap_ = nullptr;

	// �����̐F
	Vector3 ambientColor_ = { 1,1,1 };

	// �f�B�t���[�Y�̐F
	Vector3 diffuseColor_ = { 1,1,1 };

	// �X�y�L�����[�̐F
	Vector3 specularColor_ = { 1,1,1 };

	// ���s�����̔z��
	DirectionalLight dirLights_[sDirLightNum];

	// �_�����̔z��
	PointLight pointLights_[sPointLightNum];

	// �X�|�b�g���C�g�̔z��
	SpotLight spotLights_[sSpotLightNum];

	// �ۉe�̔z��
	CircleShadow circleShadows_[sCircleShadowNum];

	// �_�[�e�B�t���O
	bool dirty_ = false;

};

