#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

class PointLight
{
public: // �T�u�N���X

// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		Vector3 lightpos;
		float pad1;
		Vector3 lightcolor;
		float pad2;
		Vector3 lightatten;
		unsigned int active;
	};

public: // �����o�֐�
	/// <summary>
	/// ���C�g���W���Z�b�g
	/// </summary>
	/// <param name="lightpos">���C�g���W</param>
	inline void SetLightPos(const Vector3& lightpos) { lightpos_ = lightpos; }

	/// <summary>
	/// ���C�g���W���擾
	/// </summary>
	/// <returns>���C�g���W</returns>
	inline const Vector3& GetLightPos() { return lightpos_; }

	/// <summary>
	/// ���C�g�F���Z�b�g
	/// </summary>
	/// <param name="lightcolor">���C�g�F</param>
	inline void SetLightColor(const Vector3& lightcolor) { lightcolor_ = lightcolor; }

	/// <summary>
	/// ���C�g�F���擾
	/// </summary>
	/// <returns>���C�g�F</returns>
	inline const Vector3& GetLightColor() { return lightcolor_; }

	/// <summary>
	/// ���C�g���������W�����Z�b�g
	/// </summary>
	/// <param name="lightatten">���C�g���������W��</param>
	inline void SetLightAtten(const Vector3& lightAtten) { lightAtten_ = lightAtten; }

	/// <summary>
	/// ���C�g���������W�����擾
	/// </summary>
	/// <returns>���C�g���������W��</returns>
	inline const Vector3& GetLightAtten() { return lightAtten_; }

	/// <summary>
	/// �L���t���O���Z�b�g
	/// </summary>
	/// <param name="active">�L���t���O</param>
	inline void SetActive(bool active) { active_ = active; }

	/// <summary>
	/// �L���`�F�b�N
	/// </summary>
	/// <returns>�L���t���O</returns>
	inline bool IsActive() { return active_; }

private: // �����o�ϐ�
	// ���C�g���W�i���[���h���W�n�j
	Vector3 lightpos_ = { 0,0,0 };
	// ���C�g�F
	Vector3 lightcolor_ = { 1,1,1 };
	// ���C�g���������W��
	Vector3 lightAtten_ = { 1.0f, 1.0f, 1.0f };
	// �L���t���O
	bool active_ = false;
};

