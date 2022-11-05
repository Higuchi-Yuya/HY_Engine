#pragma once
#include<d3dcompiler.h>
#include<d3d12.h>
#include<DirectXMath.h>
#include <DirectXTex.h>
#include <string>
#pragma comment(lib,"d3d12.lib")
#include "Vector3.h"

#include <wrl.h>
using namespace DirectX;
#include"Input.h"
class Mesh
{

public:
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	
	// ���_�f�[�^�\����
	struct Vertex
	{
		XMFLOAT3 pos;    // xyz���W
		XMFLOAT3 normal; // �@���x�N�g��
		XMFLOAT2 uv;     // uv���W
	};

	struct Vertex2
	{
		XMFLOAT3 pos;
	};

	// �萔�o�b�t�@�p�f�[�^�\����  (3D�ϊ��s��)
	struct ConstBufferDataTransform {
		XMMATRIX mat; // 3D�ϊ��s��
	};

	// �萔�o�b�t�@�p�f�[�^�\���́i�}�e���A���j
	struct ConstBufferDataMaterial {
		XMFLOAT4 color; // �F (RGBA)
	};
	struct Object3d
	{
		//�萔�o�b�t�@ (�s��p)
		ComPtr<ID3D12Resource> constBuffTransform = nullptr;

		//�萔�o�b�t�@�}�b�v(�s��p)
		ConstBufferDataTransform* constMapTransform = nullptr;

		//�A�t�B���ϊ����
		XMFLOAT3 scale = { 1.0f,1.0f,1.0f };
		XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };
		XMFLOAT3 position = { 0.0f,0.0f,0.0f };

		//���[���h�ϊ��s��
		XMMATRIX matWorld{};

		//�e�I�u�W�F�N�g�ւ̃|�C���^
		Object3d* parent = nullptr;
	};

	//���e�s��
	static XMMATRIX matProjection;

	//�r���[�ϊ��s��
	static XMMATRIX matView;
	XMFLOAT3 eye = { 0, 0, -100 };  //���_���W
	XMFLOAT3 target = { 0, 0, 0 };  //�����_���W
	XMFLOAT3 up = { 0, 1, 0 };      //������x�N�g��

public:
	Mesh();

	~Mesh();

	void Mesh_Initialization(ID3D12Device* device,Vertex *vertices, unsigned short *indices, int vertices_count, int indices_count);

	void Mesh_Update(Input *input);

	void Mesh_Draw(ID3D12Device* device, int indices_count, ID3D12GraphicsCommandList* commandList);

	void InitializeObject3d(Object3d* object, ID3D12Device* device);

	void UpdateObject3d(Object3d* object, XMMATRIX& matView, XMMATRIX& matProjection);

	void DrawObject3d(Object3d* object, ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW& vdView, D3D12_INDEX_BUFFER_VIEW& ibView, UINT numIndices);

	void Mesh_InitializeLine_Line(ID3D12Device* device, Vertex2* vertices, int vertices_count);

	void Mesh_Draw_Line(int indices_count, ID3D12GraphicsCommandList* commandList);

private:
	/*void ConstantBuffer_creation(struct ConstBufferData);*/
	
	
private:
	HRESULT result;



	float R = 1.0f, G = 0.0f, B = 0.0f ,A = 1.0f;

	bool annihilation_flag = false;

	ConstBufferDataMaterial* constMapMaterial = nullptr;

	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	//�ݒ�����Ƀf�X�N���v�^�q�[�v�𐶐�
	ComPtr<ID3D12DescriptorHeap> srvHeap = nullptr;

	//�o�b�t�@�}�e���A��
	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;

	//�f�X�N���v�^�q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

	//�C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW ibView{};
	
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;

	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;

	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff = nullptr;

	ComPtr<ID3D12Resource> texBuff2 = nullptr;

	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff = nullptr;

	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff = nullptr;


	//���[���h�ϊ��s�� 0��
	XMMATRIX matWorld;
	XMMATRIX matScale;
	XMMATRIX matRot;
	XMMATRIX matTrans;

	//���[���h�ϊ��s�� 1��
	XMMATRIX matWorld1;
	XMMATRIX matScale1;
	XMMATRIX matRot1;
	XMMATRIX matTrans1;



	// �J�����̋���
	float dis = 100.0f;

	//�p�x
	XMFLOAT3 angle = { -1.5708f,0,0 };

	UINT incrementSize;

	static const size_t kObjectCount = 50;

	Object3d object3ds[kObjectCount];

};



