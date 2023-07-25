#pragma once
#include "ViewProjection.h"
#include "MathUtil.h"
#include <d3d12.h>
#include "TextureManager.h"
#include <list>

class ParticleManager
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public: // サブクラス
	//// 頂点データ構造体
	//struct VertexPosNormalUv
	//{
	//	Vector3 pos; // xyz座標
	//	Vector3 normal; // 法線ベクトル
	//	vextor2 uv;  // uv座標
	//};
	
	// 頂点データ構造体
	struct VertexPos
	{
		Vector3 pos; // xyz座標
		float scale;
		Vector4 color;
	};

	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		//Vector4 color;	// 色 (RGBA)
		Matrix4 mat;	// 3D変換行列
		Matrix4 matBillboard;//ビルボード行列
	};

	enum class Type
	{
		Normal,
	};

	//パーティクル一粒
	struct Particle
	{
		// どのように動くのかのタイプ
		Type type;

		// 座標
		Vector3 position = {};

		// 速度
		Vector3 velocity = {};

		// 加速度
		Vector3 accel = {};

		// スケール
		float scale = 1.0f;
		// 初期値
		float startScale = 1.0f;
		// 最終値
		float endScale = 0.0f;

		// 現在フレーム
		int frame = 0;
		// 終了フレーム
		int numFrame = 0;

		// 色
		Vector4 color = {};
		// 色の初期値
		Vector4 startColor = {};
		// 色の最終値
		Vector4 endColor = {};

	};

public: // 静的メンバ関数
	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 静的終了処理
	/// </summary>
	static void StaticFinalize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

private: // 静的メンバ変数
	// デバイス
	static ID3D12Device* device;
	// コマンドリスト
	static ID3D12GraphicsCommandList* cmdList;
	// ルートシグネチャ
	static ComPtr<ID3D12RootSignature> rootsignature;
	// パイプラインステートオブジェクト
	static ComPtr<ID3D12PipelineState> pipelinestate;
	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;

private:// 静的メンバ関数

	/// <summary>
	/// グラフィックパイプライン生成
	/// </summary>
	/// <returns>成否</returns>
	static void InitializeGraphicsPipeline();

	/// <summary>
	/// 頂点バッファの生成
	/// </summary>
	void InitializeVerticeBuff();

public: // メンバ関数

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(const ViewProjection& view);

	/// <summary>
	/// パーティクルのテクスチャのセット
	/// </summary>
	/// <param name="textureHandle">テクスチャハンドル</param>
	void SetTextureHandle(Texture *textureHandle);

	/// <summary>
	/// パーティクル全体のサイズを取得
	/// </summary>
	/// <returns></returns>
	size_t GetParticlesListSize() { return Particles.size(); }

	/// <summary>
	/// イージングパーティクル（out)
	/// </summary>
	/// <param name="Type">パーティクルの動きのタイプ</param>
	/// <param name="life">寿命(イージングなんでスピードにも関係する)</param>
	/// <param name="position">初期座標</param>
	/// <param name="velocity">速度</param>
	/// <param name="accel">加速度</param>
	/// <param name="startScale">初期サイズ</param>
	/// <param name="endScale">最終サイズ</param>
	/// <param name="startColor">初期色</param>
	/// <param name="endColor">最終色</param>
	void Add(Type type, int life, Vector3 position,Vector3 velocity, Vector3 accel,float startScale, float endScale, Vector4 startColor, Vector4 endColor);

	void AllDelete() { Particles.clear(); }

private: // メンバ変数

	ComPtr<ID3D12Resource> constBuff; // 定数バッファ

	// ローカルワールド変換行列
	Matrix4 matWorld;

	//パーティクル配列
	std::list<Particle>Particles;

	// パーティクルのテクスチャ
	Texture *textureHandle_ = nullptr;
};

