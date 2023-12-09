#pragma once
#include "PostRenderBase.h"

class PostEffectRipples
{
public:// サブクラス
    //定数バッファ用データ構造体
    struct ConstBufferDataRipples {
        Vector2 centerPos;  // 中心座標
        bool IsActive;      // 波紋をオンにするかどうか
        float alpha;        // 輝度：0～1
        float waveSpan;     // 波の数 多いほど細かい：0～100
        float waveFrame;    // 中心座標から広がっていく速度：0～1
        float waveScale;    // 波の間隔：0～0.1f 0.1以上だと大きくなりすぎる
    };

public:
    // コンストラクタ
    PostEffectRipples();

    // 初期化
    void Initialize();

    // 更新処理
    void Update();

    // Imguiの更新処理
    void ImguiUpdate();

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

public:
    // 定数バッファ用の変数
    Vector2 centerPos_ = { 0.5f,0.5f };
    bool IsActive_ = true;
    float alpha_ = 1.0f;
    float waveSpan_ = 100.0f;
    float waveFrame_ = 0.0f;
    float waveScale_ = 0.01f;

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



    // 定数バッファ
    ConstBufferDataRipples* constMap_ = nullptr;
};

