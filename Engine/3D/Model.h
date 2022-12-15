#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <d3d12.h>
#include <d3dx12.h>
#include <unordered_map>

class Model
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:// サブクラス
	// 頂点データ構造体
	struct VertexPosNormalUv
	{
		Vector3 pos; // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;  // uv座標
	};

	// マテリアル
	struct Material
	{
		std::string name;// マテリアル名
		Vector3 ambient; // アンビエント影響度
		Vector3 diffuse; // ディフューズ影響度
		Vector3 specular; // スペキュラー影響度
		float alpha; // アルファ
		std::string textureFilename; // テクスチャファイル名
		// コンストラクタ
		Material() {
			ambient = { 0.3f,0.3f,0.3f };
			diffuse = { 0.0f,0.0f,0.0f };
			specular = { 0.0f,0.0f,0.0f };
			alpha = 1.0f;
		}
	};

	// 定数バッファ用データ構造体B1
	struct ConstBufferDataB1
	{
		Vector3 ambient; // アンビエント係数
		float pad1; // パディング
		Vector3 diffuse; // ディフューズ係数
		float pad2; // パディング
		Vector3 specular; // スペキュラー係数
		float alpha; // アルファ
	};

public:// メンバ関数
	Model();
	~Model();

	// OBJファイルから3Dモデルを読み込む
	static Model* LoadFromOBJ(const std::string& modelname,bool smoothing = false);

	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	void LoadTexture(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// マテリアル読み込み
	/// </summary>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndexMaterial);


	// デバイスのセッター
	static void SetDevice(ID3D12Device* device) { Model::device = device; }

	// エッジ平滑化データの追加
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);

	// 平滑化された頂点法線の計算
	void CalculateSmoothedVertexNormals();

	// 頂点データの数を取得
	inline size_t GetVertexCount() { return vertices.size(); }

private:
	// 静的な関数内ではメンバ変数を呼び出せないためプライベートメンバ関数のパッケージを作った
	// LoadFromOBJの中身
	void LoadFromOBJInternal(const std::string& modelname, bool smoothing = false);

	/// <summary>
	/// デスクリプタヒープの初期化
	/// </summary>
	void InitializeDescriptorHeap();

	/// <summary>
	/// 各種バッファ生成
	/// </summary>
	void CreateBuffers();

private:// メンバ変数

	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices;

	// 頂点インデックス配列
	std::vector<unsigned short> indices;

	// マテリアル
	Material material;

	// デバイス（借りてくる）
	static ID3D12Device* device;

	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;

	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;

	// テクスチャバッファ
	ComPtr<ID3D12Resource> texbuff;

	// デスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descHeap;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;

	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView;

	// マテリアル用定数バッファ
	ComPtr<ID3D12Resource> constBuffB1; 

	// 頂点法線スムージング用データ
	std::unordered_map<unsigned short, std::vector<unsigned short>>smoothData;

	// テクスチャ番号
	uint32_t textureIndex = 0;
};