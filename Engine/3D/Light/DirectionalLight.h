#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

class DirectionalLight
{
public:// �T�u�N���X
	struct ConstBufferData
	{
		Vector3 lightv;// ���C�g�ւ̕�����\���x�N�g��
		float pad1; // �p�f�B���O
		Vector3 lightcolor;// ���C�g�̐F
		unsigned int active;
	};

public:
	DirectionalLight();
	~DirectionalLight();

	/// <summary>
	/// ���C�g�������Z�b�g
	/// </summary>
	/// <param name="lightdir">���C�g����</param>
	inline void SetLightDir(const Vector3& lightdir){lightdir_ = lightdir;lightdir_.normalize();};

	/// <summary>
	/// ���C�g�������擾
	/// </summary>
	/// <returns>���C�g����</returns>
	inline const Vector3& GetLightDir() { return lightdir_; }

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
	/// �L���t���O���Z�b�g
	/// </summary>
	/// <param name="active">�L���t���O</param>
	inline void SetActive(bool active) { active_ = active; }

	/// <summary>
	/// �L���`�F�b�N
	/// </summary>
	/// <returns>�L���t���O</returns>
	inline bool IsActive() { return active_; }

private:
	// ���C�g���������i�P�ʃx�N�g���j
	Vector3 lightdir_ = { 1,0,0 };

	// ���C�g�̐F
	Vector3 lightcolor_ = { 1.0f,1.0f,1.0f };

	// �L���t���O
	bool active_ = false;

};
