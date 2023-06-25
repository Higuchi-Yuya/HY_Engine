#pragma once
#include "Sprite.h"
#include "Input.h"

class PostEffect
{
public:
    // �R���X�g���N�^
    PostEffect();

    // ������
    void Initialize();

    // �`��R�}���h�̔��s
    void Draw(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// �V�[���`��O����
    /// </summary>
    /// <param name="cmdList">�R�}���h���X�g</param>
    void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// �V�[���`��㏈��
    /// </summary>
    /// <param name="cmdList">�R�}���h���X�g</param>
    void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

    // �f�o�C�X�̃Z�b�^�[
    static void SetDevice(ID3D12Device* device);

private:

    /// <summary>
    /// ���_�o�b�t�@�̐���
    /// </summary>
    void CreateVertBuff();

    /// <summary>
    /// �e�N�X�`������
    /// </summary>
    void CreateTex();
    
    /// <summary>
    /// SRV�쐬
    /// </summary>
    void CreateSRV();

    /// <summary>
    /// RTV�쐬
    /// </summary>
    void CreateRTV();

    /// <summary>
    /// �[�x�o�b�t�@����
    /// </summary>
    void CreateDepthBuff();

    /// <summary>
    /// DSV�쐬
    /// </summary>
    void CreateDSV();


    void CreateGraphicsPipelineState();

private:
    // �e�N�X�`���o�b�t�@
    Microsoft::WRL::ComPtr<ID3D12Resource> texBuff_[2];
    
    // �f�o�C�X�i�؂�Ă���j
    static ID3D12Device* sDevice_;

    // ���_��
    static const int kVertNum_ = 4;

    // ���_�f�[�^
    SpriteManager::Vertex vertices_[kVertNum_] = {
    {{-0.5f,-0.5f, 0.0f },{0.0f,1.0f}}, // ����
    {{-0.5f,+0.5f, 0.0f },{0.0f,0.0f}}, // ����
    {{+0.5f,-0.5f, 0.0f },{1.0f,1.0f}}, // �E��
    {{+0.5f,+0.5f, 0.0f },{1.0f,0.0f}}, // �E��
    };

    // ���_�}�b�v
    SpriteManager::Vertex* vertMap_ = nullptr;

    // ���_�o�b�t�@�r���[
    D3D12_VERTEX_BUFFER_VIEW vbView_{};

    // ���_�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_ = nullptr;

    // �萔�o�b�t�@
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_ = nullptr;

    // SRV�p�f�X�N���v�^�q�[�v
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapSRV_;

    // �[�x�o�b�t�@
    Microsoft::WRL::ComPtr <ID3D12Resource>depthBuff_;
    // RTV�p�f�X�N���v�^�q�[�v
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>descHeapRTV_;
    // DSV�p�f�X�N���v�^�q�[�v
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>descHeapDSV_;

    // �O���t�B�b�N�X�p�v���C��
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

    // ���[�g�V�O�l�`��
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

    // �e�N�X�`��
    //Texture texHandle_;

    // �m�F�pInput
    Input input;

    // ��ʃN���A�J���[
    static const float clearColor_[4];

    HRESULT result;
};

