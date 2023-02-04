#pragma once
#include <vector>
#include <unordered_map>
#include "Material.h"
class Mesh
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

public:
	// デバイスのセッター
	static void SetDevice(ID3D12Device* device);

	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList);

	// エッジ平滑化データの追加
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);

	// 平滑化された頂点法線の計算
	void CalculateSmoothedVertexNormals();

	/// <summary>
	/// 名前を取得
	/// </summary>
	/// <returns>名前</returns>
	const std::string& GetName() { return name_; }

	/// <summary>
	/// 名前をセット
	/// </summary>
	/// <param name="name">名前</param>
	void SetName(const std::string& name_);

	/// <summary>
	/// 頂点データの追加
	/// </summary>
	/// <param name="vertex">頂点データ</param>
	void AddVertex(const VertexPosNormalUv& vertex);

	/// <summary>
	/// 頂点インデックスの追加
	/// </summary>
	/// <param name="index">インデックス</param>
	void AddIndex(unsigned short index);

	// 頂点データの数を取得
	inline size_t GetVertexCount() { return vertices.size(); }

	/// <summary>
	/// マテリアルの取得
	/// </summary>
	/// <returns>マテリアル</returns>
	Material* GetMaterial() { return material; }

	/// <summary>
	/// マテリアルの割り当て
	/// </summary>
	/// <param name="material">マテリアル</param>
	void SetMaterial(Material* material);

	/// <summary>
	/// バッファの生成
	/// </summary>
	void CreateBuffers();

	/// <summary>
	/// 頂点バッファ取得
	/// </summary>
	/// <returns>頂点バッファ</returns>
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView; }

	/// <summary>
	/// インデックスバッファ取得
	/// </summary>
	/// <returns>インデックスバッファ</returns>
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView; }

	/// <summary>
	/// 頂点配列を取得
	/// </summary>
	/// <returns>頂点配列</returns>
	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices; }

	/// <summary>
	/// インデックス配列を取得
	/// </summary>
	/// <returns>インデックス配列</returns>
	inline const std::vector<unsigned short>& GetIndices() { return indices; }

private:

	// デバイス（借りてくる）
	static ID3D12Device* device;

	// 名前
	std::string name_;

	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices;

	// 頂点インデックス配列
	std::vector<unsigned short> indices;

	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;

	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView = {};

	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView = {};

	// 頂点法線スムージング用データ
	std::unordered_map<unsigned short, std::vector<unsigned short>>smoothData;

	// マテリアル
	Material* material = nullptr;
};

