#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <DirectXMath.h>
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <d3dx12.h>
#include "Model.h"
#include "LightGroup.h"
#include <vector>

/// <summary>
/// 3Dオブジェクト
/// </summary>

class Object3d
{
public:
	enum BlendMode
	{
		NONE,           // ブレンドなし
		NORMAL,         // 通常(アルファブレンド)
		ADDITION,       // 加算
		ADDITIONALPHA,  // 加算(透過あり)
		SUBTRACTION,    // 減算
		SCREEN,         // スクリーン
		BLEND_NUMMAX,
	};
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;



public: // 静的メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	static void StaticInitialize(ID3D12Device* device, int window_width, int window_height);

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
	static Object3d* Create();

	/// <summary>
	/// ライトのセット
	/// </summary>
	static void SetLight(LightGroup* light) {Object3d::light = light;}

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

	// 加算ブレンド
	static void InitializeGraphicsPipelineADDITION();

	// 加算ブレンド(透過あり)
	static void InitializeGraphicsPipelineADDITIONALPHA();

	// 減算ブレンド
	static void InitializeGraphicsPipelineSUBTRACTION();

	// スクリーン
	static void InitializeGraphicsPipelineSCREEN();

	/// <summary>
	/// ブレンドモード設定
	/// </summary>
	static void SetBlendMode(BlendMode mode);

private: // 静的メンバ変数
	// デバイス
	static ComPtr<ID3D12Device> device;

	// ライト
	static LightGroup* light;

	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestateNormal;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestateADDITION;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestateADDITIONALPHA;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestateSUBTRACTION;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestateSCREEN;

	// インプットレイアウト
	static std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout;

	static ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	static ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
	static ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

private:// 静的メンバ関数


public: // メンバ関数

	// 初期化処理
	bool Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection* viewProjection);

	/// <summary>
	/// モデルの設定
	/// </summary>
	void SetModel(Model* model) { this->model = model; }



public:// パブリック変数

	// ワールド変換データ
	WorldTransform worldTransform_;

private: // メンバ変数



	// モデル
	Model* model = nullptr;


};

