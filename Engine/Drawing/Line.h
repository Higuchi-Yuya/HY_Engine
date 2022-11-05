#pragma once
#include "Mesh.h"

class Line
{
public:
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	void Line_Initialize(ID3D12Device* device, Mesh::Vertex* vertices, unsigned short* indices, int vertices_count, int indices_count);
	void Line_Updata();
	void Line_Draw(int indices_count, ID3D12GraphicsCommandList* commandList);
private:
	HRESULT result;

	// �e�N�X�`���o�b�t�@
	ComPtr<ID3D12Resource> texBuff = nullptr;

	//�ݒ�����Ƀf�X�N���v�^�q�[�v�𐶐�
	ComPtr<ID3D12DescriptorHeap> srvHeap = nullptr;

	//�o�b�t�@�}�e���A��
	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;

	Mesh::ConstBufferDataMaterial* constMapMaterial = nullptr;

	//�f�X�N���v�^�q�[�v
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

	// ���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	// ���_�o�b�t�@
	ComPtr<ID3D12Resource> vertBuff = nullptr;

	// �C���f�b�N�X�o�b�t�@
	ComPtr<ID3D12Resource> indexBuff = nullptr;

	//�C���f�b�N�X�o�b�t�@
	D3D12_INDEX_BUFFER_VIEW ibView{};

	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;

	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle;

	UINT incrementSize;

	Mesh::Object3d object3d;

};

