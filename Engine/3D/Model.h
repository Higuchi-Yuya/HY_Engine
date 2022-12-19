#pragma once

#include <vector>
#include <unordered_map>
#include "Material.h"

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
	void Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndexMaterial);


	// デバイスのセッター
	static void SetDevice(ID3D12Device* device);

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
	/// 各種バッファ生成
	/// </summary>
	void CreateBuffers();

	/// <summary>
	/// マテリアルの登録
	/// </summary>
	/// <param name="material"></param>
	void AddMaterial(Material* material);

private:// メンバ変数

	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices;

	// 頂点インデックス配列
	std::vector<unsigned short> indices;

	//// マテリアル
	//Material material;

	// マテリアルコンテナ
	std::unordered_map<std::string, Material*> materials;

	// デバイス（借りてくる）
	static ID3D12Device* device;

	// 頂点バッファ
	ComPtr<ID3D12Resource> vertBuff;

	// インデックスバッファ
	ComPtr<ID3D12Resource> indexBuff;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView;

	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView;

	// 頂点法線スムージング用データ
	std::unordered_map<unsigned short, std::vector<unsigned short>>smoothData;


};