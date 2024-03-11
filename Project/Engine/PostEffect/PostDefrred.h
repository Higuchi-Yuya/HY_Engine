#pragma once
#include "PostRenderBase.h"

class PostDefrred
{
public:// サブクラス
//定数バッファ用データ構造体
    struct ConstBufferDataComposition {
        int texNum;
    };

private:
    enum class rootParameterIndex
    {
        TARGETSCENE,// 元のシーンの描画テクスチャマップ
        WORLDPOSMAP, // ワールド変換データマップ
        CAMERAPOSMAP,// カメラデータマップ
        NORMALMAP,// 法線データマップ
        AMBIENTMAP,// アンビエントマップ
        DIFFUSEMAP,// ディフューズマップ
        SPECULARMAP,// スペキュラーマップ
        TEXNUM,// テクスチャ番号のバッファデータ
    };
public:
    // コンストラクタ
    PostDefrred();

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
    void PostDrawScene();

public:// セッター

    // デバイスのセッター
    static void SetDevice(ID3D12Device* device);

    /// <summary>
    /// レンダーテクスチャのタイリングを設定
    /// </summary>
    /// <param name="compoTiling">タイリング</param>
    void SetCompoTiling(const Vector2& compoTiling) { compoTiling_ = compoTiling; }

    /// <summary>
    /// レンダーテクスチャのオフセットを設定
    /// </summary>
    /// <param name="compoOffset">オフセット</param>
    void SetCompoOffset(const Vector2& compoOffset) { compoOffset_ = compoOffset; }

    /// <summary>
    /// テクスチャ番号を設定
    /// </summary>
    /// <param name="texNum"></param>
    void SetTexNum(int texNum) { texNum_ = texNum; }

public:// ゲッター

    /// <summary>
    /// タイリング情報を取得
    /// </summary>
    /// <returns></returns>
    Vector2 GetTiling() { return compoTiling_; }

    /// <summary>
    /// オフセット情報を取得
    /// </summary>
    /// <returns></returns>
    Vector2 GetOffset() { return compoOffset_; }

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
    // テクスチャの数
    static const int kTexNum = 7;
    // コマンドリスト
    static ID3D12GraphicsCommandList* sCmdList_;
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

    // テクスチャのタイリング
    Vector2 compoTiling_ = { 1.0f,1.0f };

    // テクスチャのオフセット
    Vector2 compoOffset_ = { 0,0 };

    // マッピング済みアドレス
    ConstBufferDataComposition* constMap_ = nullptr;

    // テクスチャ番号
    int texNum_ = 0;

    HRESULT result;
};

