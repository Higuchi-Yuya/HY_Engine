#pragma once
#include "Sprite.h"
#include "Input.h"

class PostEffect
{
public:
    // コンストラクタ
    PostEffect();

    // 初期化
    void Initialize();

    // 描画コマンドの発行
    void Draw(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// シーン描画前処理
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void PreDrawScene(ID3D12GraphicsCommandList* cmdList);

    /// <summary>
    /// シーン描画後処理
    /// </summary>
    /// <param name="cmdList">コマンドリスト</param>
    void PostDrawScene(ID3D12GraphicsCommandList* cmdList);

    // デバイスのセッター
    static void SetDevice(ID3D12Device* device);

private:

    /// <summary>
    /// 頂点バッファの生成
    /// </summary>
    void CreateVertBuff();

    /// <summary>
    /// テクスチャ生成
    /// </summary>
    void CreateTex();
    
    /// <summary>
    /// SRV作成
    /// </summary>
    void CreateSRV();

    /// <summary>
    /// RTV作成
    /// </summary>
    void CreateRTV();

    /// <summary>
    /// 深度バッファ生成
    /// </summary>
    void CreateDepthBuff();

    /// <summary>
    /// DSV作成
    /// </summary>
    void CreateDSV();


    void CreateGraphicsPipelineState();

private:
    // テクスチャバッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> texBuff_[2];
    
    // デバイス（借りてくる）
    static ID3D12Device* sDevice_;

    // 頂点数
    static const int kVertNum_ = 4;

    // 頂点データ
    SpriteManager::Vertex vertices_[kVertNum_] = {
    {{-0.5f,-0.5f, 0.0f },{0.0f,1.0f}}, // 左下
    {{-0.5f,+0.5f, 0.0f },{0.0f,0.0f}}, // 左上
    {{+0.5f,-0.5f, 0.0f },{1.0f,1.0f}}, // 右下
    {{+0.5f,+0.5f, 0.0f },{1.0f,0.0f}}, // 右上
    };

    // 頂点マップ
    SpriteManager::Vertex* vertMap_ = nullptr;

    // 頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vbView_{};

    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_ = nullptr;

    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_ = nullptr;

    // SRV用デスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapSRV_;

    // 深度バッファ
    Microsoft::WRL::ComPtr <ID3D12Resource>depthBuff_;
    // RTV用デスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>descHeapRTV_;
    // DSV用デスクリプタヒープ
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>descHeapDSV_;

    // グラフィックスパプライン
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

    // ルートシグネチャ
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

    // テクスチャ
    //Texture texHandle_;

    // 確認用Input
    Input input;

    // 画面クリアカラー
    static const float clearColor_[4];

    HRESULT result;
};

