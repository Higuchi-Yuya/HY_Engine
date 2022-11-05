#pragma once
#include<d3dcompiler.h>
#include<d3d12.h>
#include <string>
#include <wrl.h>

class Render_basic
{
public:
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
	static Render_basic* GetInstance();
	static void DestroyInstance();
	void Initialization(ID3D12Device* device);
	void Initialization2(ID3D12Device* device);
	ID3D12PipelineState* GetPipelineState()const;
	ID3D12PipelineState* GetPipelineState2()const;
	ID3D12RootSignature* GetRootSignature()const;


private:
	Render_basic() = default;
	~Render_basic() = default;

	Render_basic(const Render_basic&) = delete;
	Render_basic& operator=(const Render_basic&) = delete;
	Render_basic(const Render_basic&&) = delete;
	Render_basic& operator=(const Render_basic&&) = delete;

	ID3DBlob* vsBlob = nullptr; // ���_�V�F�[�_�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr; // �s�N�Z���V�F�[�_�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr; // �G���[�I�u�W�F�N�g

	D3D12_INPUT_ELEMENT_DESC inputLayout[3];

	HRESULT result;
	// �p�C�v�����X�e�[�g�̐���
	ComPtr<ID3D12PipelineState> pipelineStage = nullptr;
	ComPtr<ID3D12PipelineState> pipelineStage2 = nullptr;
	// ���[�g�V�O�l�`��
	ComPtr<ID3D12RootSignature> rootSignature = nullptr;
};

