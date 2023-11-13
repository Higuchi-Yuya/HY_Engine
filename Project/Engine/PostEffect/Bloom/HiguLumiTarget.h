#pragma once
#include "PostRenderBase.h"

class HiguLumiTarget
{
public:
    // コンストラクタ
    HiguLumiTarget();

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

public:// セッター

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
    Microsoft::WRL::ComPtr<ID3D12Resource> texBuff_;

    // デバイス（借りてくる）
    static ID3D12Device* sDevice_;

    // 頂点数
    static const int kVertNum_ = 4;

    // ハンドル
    Handles handles_;

    // 頂点データ
    SpriteManager::Vertex vertices_[kVertNum_] = {
    {{-1.0f,-1.0f, 0.0f },{0.0f,1.0f}}, // 左下
    {{-1.0f,+1.0f, 0.0f },{0.0f,0.0f}}, // 左上
    {{+1.0f,-1.0f, 0.0f },{1.0f,1.0f}}, // 右下
    {{+1.0f,+1.0f, 0.0f },{1.0f,0.0f}}, // 右上
    };

    // 頂点マップ
    SpriteManager::Vertex* vertMap_ = nullptr;

    // 頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vbView_{};

    // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_ = nullptr;

    // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_ = nullptr;

    // 深度バッファ
    Microsoft::WRL::ComPtr <ID3D12Resource>depthBuff_ = nullptr;

    // グラフィックスパプライン
    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;

    // ルートシグネチャ
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

    std::unique_ptr<ShaderObj> vsShader_;// 頂点シェーダー
    std::unique_ptr<ShaderObj> psShader_;// ピクセルシェーダー

    // 画面クリアカラー
    static const float clearColor_[4];

    HRESULT result;
};

