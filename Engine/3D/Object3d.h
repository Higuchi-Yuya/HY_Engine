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

/// <summary>
/// 3Dオブジェクト
/// </summary>

class Object3d
{
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
	static ComPtr<ID3D12PipelineState> pipelinestate;

private:// 静的メンバ関数

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static void InitializeGraphicsPipeline();

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

