#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <DirectXMath.h>

class CircleShadow
{
public: // �T�u�N���X

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		Vector3 dir;
		float pad1;
		Vector3 casterPos;
		float distanceCasterLight;
		Vector3 atten;
		float pad2;
		Vector2 factorAngleCos;
		unsigned int active;
		float pad3;
	};

public: // �����o�֐�
	/// <summary>
	/// �������Z�b�g
	/// </summary>
	/// <param name="lightdir">����</param>
	inline void SetDir(const Vector3& dir) { dir_ = dir; dir_.normalize(); }

	/// <summary>
	/// ���C�g�������擾
	/// </summary>
	/// <returns>���C�g����</returns>
	inline const Vector3& GetDir() { return dir_; }

	/// <summary>
	/// �L���X�^�[���W���Z�b�g
	/// </summary>
	/// <param name="lightpos">�L���X�^�[���W</param>
	inline void SetCasterPos(const Vector3& casterPos) { casterPos_ = casterPos; }

	/// <summary>
	/// �L���X�^�[���W���擾
	/// </summary>
	/// <returns>�L���X�^�[���W</returns>
	inline const Vector3& GetCasterPos() { return casterPos_; }

	/// <summary>
	/// �L���X�^�[�ƃ��C�g�̋������Z�b�g
	/// </summary>
	/// <param name="lightpos">�L���X�^�[�ƃ��C�g�̋���</param>
	inline void SetDistanceCasterLight(float distanceCasterLight) { distanceCasterLight_ = distanceCasterLight; }

	/// <summary>
	/// �L���X�^�[�ƃ��C�g�̋������擾
	/// </summary>
	/// <returns>�L���X�^�[�ƃ��C�g�̋���</returns>
	inline float GetDistanceCasterLight() { return distanceCasterLight_; }

	/// <summary>
	/// ���������W�����Z�b�g
	/// </summary>
	/// <param name="lightatten">���C�g���������W��</param>
	inline void SetAtten(const Vector3& atten) { atten_ = atten; }

	/// <summary>
	/// ���������W�����擾
	/// </summary>
	/// <returns>���C�g���������W��</returns>
	inline const Vector3& GetAtten() { return atten_; }

	/// <summary>
	/// �����p�x���Z�b�g
	/// </summary>
	/// <param name="lightFactorAngle">x:�����J�n�p�x y:�����I���p�x[degree]</param>
	inline void SetFactorAngle(const Vector2& factorAngle) {
		factorAngleCos_.x = cosf(DirectX::XMConvertToRadians(factorAngle.x));
		factorAngleCos_.y = cosf(DirectX::XMConvertToRadians(factorAngle.y));
	}

	/// <summary>
	/// �����p�x���擾
	/// </summary>
	/// <returns>�����p�x</returns>
	inline const Vector2& GetFactorAngleCos() { return factorAngleCos_; }

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
	// �����i�P�ʃx�N�g���j
	Vector3 dir_ = { 1,0,0 };
	// �L���X�^�[�ƃ��C�g�̋���
	float distanceCasterLight_ = 100.0f;
	// �L���X�^�[���W�i���[���h���W�n�j
	Vector3 casterPos_ = { 0,0,0 };
	// ���������W��
	Vector3 atten_ = { 0.5f, 0.6f, 0.0f };
	// �����p�x
	Vector2 factorAngleCos_ = { 0.2f, 0.5f };
	// �L���t���O
	bool active_ = false;
};

