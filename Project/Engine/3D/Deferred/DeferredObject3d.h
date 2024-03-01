#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include "Fog.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <d3dx12.h>
#include "Model.h"
#include <vector>
#include "Dissolve.h"
#include "ShaderObj.h"
#include <PostRenderBase.h>

class DeferredObject3d
{
private:
	enum class rootParameterIndex
	{
		BODYTEXTURE,// 本体に貼るテクスチャ
		WORLDTRANS, // ワールド変換データ
		VIEWPROJECTION,// ビュープロジェクションのデータ
		MATERIALDATA,// マテリアルのバッファデータ
	};

	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // 静的メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// ディファードオブジェクトが書き込まれているレンダーテクスチャを描画
	/// </summary>
	static void DrawDeferredObjRenderTex();

	/// <summary>
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static DeferredObject3d* Create();

	// スタティック系を解放する関数
	static void StaticFinalize();

private:

	/// <summary>
	/// ルートシグネチャの生成
	/// </summary>
	static void InitializeRootSignature();

	/// <summary>
	/// シェーダーの読み込み
	/// </summary>
	static void InitializeShader();

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>

	// ノーマルブレンド
	static void InitializeGraphicsPipelineNormal();

	/// <summary>
	/// 頂点バッファの生成
	/// </summary>
	static void CreateVertBuff();

	/// <summary>
	/// テクスチャ生成
	/// </summary>
	static void CreateTex();

	/// <summary>
	/// SRV作成
	/// </summary>
	static void CreateSRV();

	/// <summary>
	/// RTV作成
	/// </summary>
	static void CreateRTV();

	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	static void CreateDepthBuff();

	/// <summary>
	/// DSV作成
	/// </summary>
	static void CreateDSV();





private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* sDevice_;

	// コマンドリスト
	static ID3D12GraphicsCommandList* sCmdList_;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> sRootsignature_;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> sPipelinestateNormal_;

	// インプットレイアウト
	static std::vector<D3D12_INPUT_ELEMENT_DESC> sInputLayout_;

	static ComPtr<ID3DBlob> sVsBlob_; // 頂点シェーダオブジェクト
	static ComPtr<ID3DBlob> sPsBlob_;	// ピクセルシェーダオブジェクト
	static ComPtr<ID3DBlob> sErrorBlob_; // エラーオブジェクト

	static ShaderObj* sVsShader_;// 頂点シェーダー
	static ShaderObj* sPsShader_;// ピクセルシェーダー


	// テクスチャバッファ
	static Microsoft::WRL::ComPtr<ID3D12Resource> texBuff_[6];

	// 頂点数
	static const int kVertNum_ = 4;

	// ハンドル
	static Handles handles_[6];

	// 頂点データ
	static SpriteManager::Vertex vertices_[kVertNum_]; 

	// 頂点マップ
	static SpriteManager::Vertex* vertMap_;

	// 頂点バッファビュー
	static D3D12_VERTEX_BUFFER_VIEW vbView_;

	// 頂点バッファ
	static Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;

	// 深度バッファ
	static Microsoft::WRL::ComPtr <ID3D12Resource>depthBuff_;

	// 画面クリアカラー
	static const float clearColor_[4];

	static HRESULT result;

public: // メンバ関数

	// デストラクタ
	~DeferredObject3d();

	// 初期化処理
	bool Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 行列の更新
	/// </summary>
	/// <returns></returns>
	void UpdateWorldMatrix();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection * viewProjection);

	/// <summary>
	/// ワールド行列の取得
	/// </summary>
	const Matrix4& GetMatWorld() { return worldTransform_.matWorld_; }

	/// <summary>
	/// モデルの設定
	/// </summary>
	void SetModel(Model * model);

public:// パブリック変数

	// ワールド変換データ
	WorldTransform worldTransform_;

private: // メンバ変数
	// モデル
	Model* model_ = nullptr;


protected:// メンバ変数
	// クラス名（デバッグ用）
	const char* name_ = nullptr;
};

