#pragma once

#include "Mesh.h"

class Model
{
private: // エイリアス
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:// メンバ関数
	Model();
	~Model();

	// OBJファイルから3Dモデルを読み込む
	static Model* LoadFromOBJ(const std::string& modelname,bool smoothing = false);

	/// <summary>
	/// マテリアル読み込み
	/// </summary>
	void LoadMaterial(const std::string& directoryPath, const std::string& filename);

	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList, uint32_t materialRootIndex,uint32_t bodyTexRootIndex);


	// デバイスのセッター
	static void SetDevice(ID3D12Device* device);

	/// <summary>
	/// メッシュコンテナを取得
	/// </summary>
	/// <returns>メッシュコンテナ</returns>
	inline const std::vector<Mesh*>& GetMeshes() { return meshes_; }
	
	/// <summary>
	/// モデル内の一番小さい頂点ポジションの取得
	/// </summary>
	/// <returns></returns>
	inline const Vector3& GetMinVertex() { return minVertex_; }

	/// <summary>
	/// モデル内の一番大きい頂点ポジションの取得
	/// </summary>
	/// <returns></returns>
	inline const Vector3& GetMaxVertex() { return maxVertex_; }

private:
	// 静的な関数内ではメンバ変数を呼び出せないためプライベートメンバ関数のパッケージを作った
	// LoadFromOBJの中身
	void LoadFromOBJInternal(const std::string& modelname, bool smoothing = false);

	/// <summary>
	/// 各種バッファ生成
	/// </summary>
	void CreateBuffers();

	/// <summary>
	/// マテリアルの登録
	/// </summary>
	/// <param name="material"></param>
	void AddMaterial(Material* material);

	/// <summary>
	/// モデル内の頂点で最も大きいのと小さい頂点を登録
	/// </summary>
	/// <param name="vertexPos"></param>
	void AddMinMaxVertex(Vector3 vertexPos);

private:// メンバ変数

	// 名前
	std::string name_;

	// メッシュコンテナ
	std::vector<Mesh*> meshes_;

	// マテリアルコンテナ
	std::unordered_map<std::string, Material*> materials_;

	// デフォルトマテリアル
	Material* defaultMaterial_ = nullptr;

	// デバイス（借りてくる）
	static ID3D12Device* sDevice_;

	// モデル内にある最も小さい頂点
	bool isFirstAddMinMax;
	Vector3 minVertex_;

	// モデル内にある最も大きい頂点
	Vector3 maxVertex_;


};