#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include <d3dx12.h>
#include "WorldTransform.h"
#include <Vector2.h>
#include"TextureManager.h"
#include "ViewProjection.h"

/// <summary>
/// 3Dオブジェクト
/// </summary>
class BillboardTex
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public: // サブクラス
	// 頂点データ構造体
	struct VertexPosNormalUv
	{
		Vector3 pos; // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;  // uv座標
	};

	enum BillboardType
	{
		AllAxisBillboard,// 全方位ビルボード
		YAxisBillboard,// Y軸ビルボード
	};

private: // 定数

	static const int vertexCount = 4;		// 頂点数
	static const int indexCount = 3 * 2;    // インデックス数

public: // 静的メンバ関数
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 解放処理
	/// </summary>
	static void StaticFinalize();

	/// <summary>
	/// 共通ビュープロジェクションを設定
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション</param>
	static void SetViewProjection(ViewProjection* viewProjection);

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
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static BillboardTex* Create();

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	void LoadTexture(std::string fileName);
private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* sDevice_;
	// コマンドリスト
	static ID3D12GraphicsCommandList* sCmdList_;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> sRootsignature_;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> sPipelinestate_;
	// 頂点バッファ
	static ComPtr<ID3D12Resource> sVertBuff_;
	// インデックスバッファ
	static ComPtr<ID3D12Resource> sIndexBuff_;

	// 頂点バッファビュー
	static D3D12_VERTEX_BUFFER_VIEW vbView;
	// インデックスバッファビュー
	static D3D12_INDEX_BUFFER_VIEW ibView;
	// 頂点データ配列
	static VertexPosNormalUv vertices[vertexCount];
	// 頂点インデックス配列
	//static unsigned short indices[planeCount * 3];
	static unsigned short indices[indexCount];


private:// 静的メンバ関数
	/// <summary>
	/// デスクリプタヒープの初期化
	/// </summary>
	static void InitializeDescriptorHeap();

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static void InitializeGraphicsPipeline();


	/// <summary>
	/// モデル作成
	/// </summary>
	static void CreateModel();

	/// <summary>
	/// ビュー行列を更新
	/// </summary>
	void UpdateViewMatrix();

public: // メンバ関数
	bool Initialize();
	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// テクスチャの設定
	/// </summary>
	/// <param name="tex"></param>
	void SetTexture(Texture* tex);

public:
	// ワールドデータ
	WorldTransform worldTransform_;

private: // メンバ変数

	// 持ってくるビュープロジェクション
	static ViewProjection* sViewProjection_;

	// ビルボードオブジェのテクスチャ
	std::unique_ptr<Texture> textureHandle_ = nullptr;

	// ビルボード行列
	std::unique_ptr<Matrix4> matBillboard_ = nullptr;

	// ビルボードの種類
	BillboardType bType_ = AllAxisBillboard;

};