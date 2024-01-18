#pragma once

#include <vector>
#include <unordered_map>
#include "Material.h"

// ノード
struct Node
{
	//名前
	std::string name;
	//ローカル変形行列
	Matrix4 transform;
	//グローバル変形行列
	Matrix4 globalTransform;
	//親ノード
	Node* parent = nullptr;
	//子ノード
	std::vector<Node*>childrens;

};

class Mesh
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public:// サブクラス
	friend class FbxLoader;
	friend class FbxModel;
	static const int sMAX_BONE_INDICES = 4;

	// 骨
	struct Bone
	{
		//名前
		std::string name;

		Matrix4 matrix;
		Matrix4 animationMatrix;
		Matrix4 offsetMatirx;

		UINT index;

	};

	// 頂点データ構造体
	struct VertexPosNormalUv
	{
		Vector3 pos; // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;  // uv座標

		//uint32_t boneIndex[sMAX_BONE_INDICES];
		//float boneWeight[sMAX_BONE_INDICES];
	};



public:
	// デバイスのセッター
	static void SetDevice(ID3D12Device* device);

	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList);
	void DrawFBX(ID3D12GraphicsCommandList* cmdList,Texture texHandle);

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
	void SetName(const std::string& name);

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
	inline size_t GetVertexCount() { return vertices_.size(); }

	/// <summary>
	/// マテリアルの取得
	/// </summary>
	/// <returns>マテリアル</returns>
	Material* GetMaterial() { return material_; }

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
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() { return vbView_; }

	/// <summary>
	/// インデックスバッファ取得
	/// </summary>
	/// <returns>インデックスバッファ</returns>
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() { return ibView_; }

	/// <summary>
	/// 頂点配列を取得
	/// </summary>
	/// <returns>頂点配列</returns>
	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices_; }

	/// <summary>
	/// インデックス配列を取得
	/// </summary>
	/// <returns>インデックス配列</returns>
	inline const std::vector<unsigned short>& GetIndices() { return indices_; }

private:

	// デバイス（借りてくる）
	static ID3D12Device* sDevice_;

	// 名前
	std::string name_;

	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices_;

	// 頂点インデックス配列
	std::vector<unsigned short> indices_;

	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff_;

	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff_;

	//ボーンバッファ
	ComPtr<ID3D12Resource> BoneBuff_;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};

	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};

	// 頂点法線スムージング用データ
	std::unordered_map<unsigned short, std::vector<unsigned short>>smoothData_;

	// マテリアル
	Material* material_ = nullptr;

	// ボーン
	std::unordered_map<std::string, Bone*> bones;

	std::vector<Bone> vecBones;

	// ノード
	Node* node = nullptr;

	// マッピング済みアドレス
	Bone* constMap = nullptr;
};

