#pragma once
#include"Vector3.h"
#include "Vector4.h"
#include"Matrix4.h"
#include<d3d12.h>
#include<wrl.h>


class WorldTransform
{
public:// �T�u�N���X
	//�萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferDataWorldTransform {
		Vector4 color;
		Matrix4 matWorld;// �s��
	};
public:// �����o�֐�
	// �ÓI������
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// ������
	/// </summary>
	void Initialize();

	/// <summary>
	/// �s����X�V����
	/// </summary>
	void UpdateMatrix();

	// �o�b�t�@�̃Q�b�^�[
	ID3D12Resource *GetBuff() { return constBuff.Get(); }

private:// �v���C�x�[�g�֐�
	/// <summary>
	/// �萔�o�b�t�@����
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// �}�b�s���O����
	/// </summary>
	void Map();

public:// �p�u���b�N�ϐ�
	// ���[�J���X�P�[��
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };

	// X,Y,Z�����̃��[�J����]�p
	Vector3 rotation_ = { 0, 0, 0 };

	// ���[�J�����W
	Vector3 position_ = { 0, 0, 0 };

	// �F
	Vector4 color_ = { 1,1,1,1 };

	// ���[�J�� �� ���[���h�ϊ��s��
	Matrix4 matWorld_;

	// �e�ƂȂ郏�[���h�ϊ��ւ̃|�C���^
	const WorldTransform* parent_ = nullptr;

private:// �����o�ϐ�
	// �f�o�C�X�i�؂�Ă���j
	static Microsoft::WRL::ComPtr<ID3D12Device> device_;

	// �萔�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;

	// �}�b�s���O�ς݃A�h���X
	ConstBufferDataWorldTransform* constMap = nullptr;
};
