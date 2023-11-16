#pragma once
#include "PostRenderBase.h"

class Vignette
{
public:// サブクラス
    //定数バッファ用データ構造体
    struct ConstBufferDataVignette{
        Vector4 vignetteColor;
        float vignetteRadius;
        float vignetteFeather;
    };
public:
    // コンストラクタ
    Vignette();

    // 初期化
    void Initialize();

    // 更新処理
    void Update();

    // IMGUIの処理
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

public:// ゲッター

    /// <summary>
    /// ビネットの色を取得
    /// </summary>
    /// <returns></returns>
    inline Vector4& GetVignetteColor() { return vignetteColor_; }

    /// <summary>
    /// ビネットの広さを取得
    /// </summary>
    /// <returns></returns>
    inline float& GetVignetteRadius() { return vignetteRadius_; }

    /// <summary>
    /// ビネットの減衰を取得
    /// </summary>
    /// <returns></returns>
    inline float& GetVignetteFeather() { return vignetteFeather_; }

public:// セッター

    /// <summary>
    /// ビネットの色を設定
    /// </summary>
    /// <param name="vignetteColor">ビネットの色</param>
    inline void SetVignetteColor(const Vector4& vignetteColor) { vignetteColor_ = vignetteColor;}

    /// <summary>
    /// ビネットの広さを設定
    /// </summary>
    /// <param name="vignetteRange">ビネットの広さ</param>
    inline void SetVignetteRadius(const float& vignetteRadius) { vignetteRadius_ = vignetteRadius;}

    /// <summary>
    /// ビネットの減衰を設定
    /// </summary>
    /// <param name="vignetteRadius">ビネットの減衰</param>
    inline void SetVignetteFeather(const float& vignetteFeather) { vignetteFeather_ = vignetteFeather; }

public:// 静的セッター

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

    // ビネットの色
    Vector4 vignetteColor_ = { 0,0,0,1.0f };

    // ビネットの広さ
    float vignetteRadius_ = 0.5f;

    // ビネットの減衰
    float vignetteFeather_ = 0.5f;

    // マッピング済みアドレス
    ConstBufferDataVignette* constMap_ = nullptr;

    HRESULT result;

};

