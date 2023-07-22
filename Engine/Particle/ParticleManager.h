#pragma once
#include "ViewProjection.h"
#include "MathUtil.h"
#include <d3d12.h>
#include "TextureManager.h"
#include <list>

class ParticleManager
{
private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public: // �T�u�N���X
	//// ���_�f�[�^�\����
	//struct VertexPosNormalUv
	//{
	//	Vector3 pos; // xyz���W
	//	Vector3 normal; // �@���x�N�g��
	//	vextor2 uv;  // uv���W
	//};
	
	// ���_�f�[�^�\����
	struct VertexPos
	{
		Vector3 pos; // xyz���W
		float scale;
		Vector4 color;
	};

	// �萔�o�b�t�@�p�f�[�^�\����
	struct ConstBufferData
	{
		//Vector4 color;	// �F (RGBA)
		Matrix4 mat;	// 3D�ϊ��s��
		Matrix4 matBillboard;//�r���{�[�h�s��
	};

	enum class Type
	{
		Normal,
	};

	//�p�[�e�B�N���ꗱ
	struct Particle
	{
		// �ǂ̂悤�ɓ����̂��̃^�C�v
		Type type;

		// ���W
		Vector3 position = {};

		// ���x
		Vector3 velocity = {};

		// �����x
		Vector3 accel = {};

		// �X�P�[��
		float scale = 1.0f;
		// �����l
		float startScale = 1.0f;
		// �ŏI�l
		float endScale = 0.0f;

		// ���݃t���[��
		int frame = 0;
		// �I���t���[��
		int numFrame = 0;

		// �F
		Vector4 color = {};
		// �F�̏����l
		Vector4 startColor = {};
		// �F�̍ŏI�l
		Vector4 endColor = {};

	};

public: // �ÓI�����o�֐�
	/// <summary>
	/// �ÓI������
	/// </summary>
	/// <param name="device">�f�o�C�X</param>
	/// <param name="window_width">��ʕ�</param>
	/// <param name="window_height">��ʍ���</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// �ÓI�I������
	/// </summary>
	static void StaticFinalize();

	/// <summary>
	/// �`��O����
	/// </summary>
	/// <param name="cmdList">�`��R�}���h���X�g</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// �`��㏈��
	/// </summary>
	static void PostDraw();

private: // �ÓI�����o�ϐ�
	// �f�o�C�X
	static ID3D12Device* device;
	// �R�}���h���X�g
	static ID3D12GraphicsCommandList* cmdList;
	// ���[�g�V�O�l�`��
	static ComPtr<ID3D12RootSignature> rootsignature;
	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g
	static ComPtr<ID3D12PipelineState> pipelinestate;
	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff;
	// ���_�o�b�t�@�r���[
	D3D12_VERTEX_BUFFER_VIEW vbView;

private:// �ÓI�����o�֐�

	/// <summary>
	/// �O���t�B�b�N�p�C�v���C������
	/// </summary>
	/// <returns>����</returns>
	static void InitializeGraphicsPipeline();

	/// <summary>
	/// ���_�o�b�t�@�̐���
	/// </summary>
	void InitializeVerticeBuff();

public: // �����o�֐�

	/// <summary>
	/// ����������
	/// </summary>
	void Initialize();

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update();

	/// <summary>
	/// �`��
	/// </summary>
	void Draw(const ViewProjection& view);

	/// <summary>
	/// �p�[�e�B�N���̃e�N�X�`���̃Z�b�g
	/// </summary>
	/// <param name="textureHandle">�e�N�X�`���n���h��</param>
	void SetTextureHandle(Texture *textureHandle);

	/// <summary>
	/// �p�[�e�B�N���S�̂̃T�C�Y���擾
	/// </summary>
	/// <returns></returns>
	size_t GetParticlesListSize() { return Particles.size(); }

	/// <summary>
	/// �C�[�W���O�p�[�e�B�N���iout)
	/// </summary>
	/// <param name="Type">�p�[�e�B�N���̓����̃^�C�v</param>
	/// <param name="life">����(�C�[�W���O�Ȃ�ŃX�s�[�h�ɂ��֌W����)</param>
	/// <param name="position">�������W</param>
	/// <param name="velocity">���x</param>
	/// <param name="accel">�����x</param>
	/// <param name="startScale">�����T�C�Y</param>
	/// <param name="endScale">�ŏI�T�C�Y</param>
	/// <param name="startColor">�����F</param>
	/// <param name="endColor">�ŏI�F</param>
	void Add(Type type, int life, Vector3 position,Vector3 velocity, Vector3 accel,float startScale, float endScale, Vector4 startColor, Vector4 endColor);

	void AllDelete() { Particles.clear(); }

private: // �����o�ϐ�

	ComPtr<ID3D12Resource> constBuff; // �萔�o�b�t�@

	// ���[�J�����[���h�ϊ��s��
	Matrix4 matWorld;

	//�p�[�e�B�N���z��
	std::list<Particle>Particles;

	// �p�[�e�B�N���̃e�N�X�`��
	Texture *textureHandle_ = nullptr;
};

