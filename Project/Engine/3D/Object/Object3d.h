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
#include "LightGroup.h"
#include <vector>

#include "Dissolve.h"
#include "CollisionInfo.h"
#include "ShaderObj.h"

//#include "BaseCollider.h"

/// <summary>
/// 3Dオブジェクト
/// </summary>
class BaseCollider;

enum class rootParameterIndex
{
	BODYTEXTURE,// 本体に貼るテクスチャ
	DISSOLVETEX,// ディゾルブ用のテクスチャ
	WORLDTRANS, // ワールド変換データ
	VIEWPROJECTION,// ビュープロジェクションのデータ
	MATERIALDATA,// マテリアルのバッファデータ
	LIGHTDATA,// ライトのバッファデータ
	FOGDATA,// フォグのバッファデータ
	DISSOLVEDATA,// ディゾルブのバッファデータ
};

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
	/// 3Dオブジェクト生成
	/// </summary>
	/// <returns></returns>
	static Object3d* Create();

	/// <summary>
	/// ライトのセット
	/// </summary>
	static void SetLight(LightGroup* light) {Object3d::sLight_ = light;}

	/// <summary>
	/// フォグのセット
	/// </summary>
	static void SetFog(Fog* fog) { Object3d::sFog_ = fog; }

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

	// スタティック系を解放する関数
	static void StaticFinalize();

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* sDevice_;

	// ライト
	static LightGroup* sLight_;

	// フォグ
	static Fog* sFog_;

	// コマンドリスト
	static ID3D12GraphicsCommandList* sCmdList_;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> sRootsignature_;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> sPipelinestateNormal_;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> sPipelinestateADDITION_;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> sPipelinestateADDITIONALPHA_;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> sPipelinestateSUBTRACTION_;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> sPipelinestateSCREEN_;

	// インプットレイアウト
	static std::vector<D3D12_INPUT_ELEMENT_DESC> sInputLayout_;
	
	static ComPtr<ID3DBlob> sVsBlob_; // 頂点シェーダオブジェクト
	static ComPtr<ID3DBlob> sPsBlob_;	// ピクセルシェーダオブジェクト
	static ComPtr<ID3DBlob> sErrorBlob_; // エラーオブジェクト

	static ShaderObj *sVsShader_;// 頂点シェーダー
	static ShaderObj *sPsShader_;// ピクセルシェーダー



private:// 静的メンバ関数


public: // メンバ関数

	// コンストラクタ
	Object3d() = default;

	// デストラクタ
	virtual ~Object3d();

	// 初期化処理
	virtual bool Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	virtual void Update();
	
	/// <summary>
	/// 行列の更新
	/// </summary>
	/// <returns></returns>
	void UpdateWorldMatrix();
	
	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw(ViewProjection* viewProjection);

	/// <summary>
	/// ワールド行列の取得
	/// </summary>
	const Matrix4& GetMatWorld() { return worldTransform_.matWorld_; }

	/// <summary>
	/// モデルの設定
	/// </summary>
	void SetModel(Model* model);

	/// <summary>
	/// コライダーのセット
	/// </summary>
	void SetCollider(BaseCollider* collider);

	/// <summary>
	/// 衝突時コールバック関数
	/// </summary>
	virtual void OnCollision(const CollisionInfo&info){}

public:// パブリック変数

	// ワールド変換データ
	WorldTransform worldTransform_;

	// ディゾルブ
	Dissolve dissolve_;

private: // メンバ変数
	// モデル
	Model* model_ = nullptr;

	
protected:// メンバ変数
	// クラス名（デバッグ用）
	const char* name_ = nullptr;

	// コライダー
	BaseCollider* collider_ = nullptr;

};

