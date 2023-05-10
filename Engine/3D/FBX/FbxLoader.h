#pragma once

//#include "FbxModel.h"

#include <d3d12.h>
#include <d3dx12.h>
#include <string>

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<assimp/cimport.h>

class FbxLoader
{
private: // エイリアス
	// std::を省略
	using string = std::string;

public: // 定数
	// モデル格納ルートパス
	static const string baseDirectory;

public:// メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static FbxLoader* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device">D3D12デバイス</param>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// 後始末
	/// </summary>
	void Finalize();

	/// <summary>
	/// ファイルからFBXモデル読込
	/// </summary>
	/// <param name="modelName">モデル名</param>
	//FbxModel* LoadModelFromFile(const string& modelName);

private:// メンバ変数

	// privateなコンストラクタ（シングルトンパターン）
	FbxLoader() = default;
	// privateなデストラクタ（シングルトンパターン）
	~FbxLoader() = default;
	// コピーコンストラクタを禁止（シングルトンパターン）
	FbxLoader(const FbxLoader & obj) = delete;
	// コピー代入演算子を禁止（シングルトンパターン）
	void operator=(const FbxLoader & obj) = delete;

	// インスタンス用のローダー
	static FbxLoader* fbxLoader_;

	// D3D12デバイス（借りて来るものの入れ物）
	ID3D12Device* device = nullptr;

	// 設定の状態を表すフラグ
	const UINT flag =
		aiProcess_Triangulate | //三角面化
		aiProcess_CalcTangentSpace | //接線ベクトル生成
		aiProcess_GenSmoothNormals | //スムージングベクトル生成
		aiProcess_GenUVCoords | //非マッピングを適切なUV座標に変換
		aiProcess_RemoveRedundantMaterials | //冗長なマテリアルを削除
		aiProcess_OptimizeMeshes | //メッシュ数を最適化
		aiProcess_MakeLeftHanded | //ノードを左手座標系に
		aiProcess_GenBoundingBoxes | //AABBを生成
		aiProcess_JoinIdenticalVertices |//インデックスを生成
		aiProcess_LimitBoneWeights;//各頂点が影響を受けるボーンを4に制限

	// シーン
	const aiScene* mScene;
};

