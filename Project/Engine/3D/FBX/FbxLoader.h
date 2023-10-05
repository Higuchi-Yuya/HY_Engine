#pragma once


#include "FbxModel.h"
#include "Texture.h"


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
	static const string sBaseDirectory;

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
	FbxModel* LoadModelFromFile(const string& modelName);

	void ParseSkin(FbxModel* model, aiMesh* fbxMesh);

	void GetNodeNum(const aiNode* node, UINT32& num);

private:// プライベート関数
	
	/// <summary>
	/// 再帰的にノード構成を解析
	/// </summary>
	/// <param name="model">読み込み先モデルオブジェクト</param>
	/// <param name="fbxNode">解析対象のノード</param>
	/// <param name="parent">親ノード</param>
	void ParseNodeRecursive(FbxModel* model, aiNode* fbxNode, Node* parent = nullptr);

	/// <summary>
	/// メッシュ読み取り
	/// </summary>
	/// <param name="model">読み込み先モデルオブジェクト</param>
	/// <param name="fbxNode">解析対象のノード</param>
	void ParseMesh(FbxModel* model, aiMesh* fbxNode);
	
	// 頂点座標読み取り
	void ParseMeshVertices(FbxModel* model, aiMesh* fbxMesh);
	
	// 面情報読み取り
	void ParseMeshFaces(FbxModel* model, aiMesh* fbxMesh);
	
	// マテリアル読み取り
	void ParseMaterial(FbxModel* model, aiMesh* fbxMesh, aiMaterial* aimaterial);

	// ディレクトリを含んだファイルパスからファイル名を抽出する
	std::string ExtractFileName(const std::string& path);

	Texture LoadMatrixerialTextures(aiMaterial* cmatrix, aiTextureType type, std::string typeName, const aiScene* scene_, const std::string& modelName);

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
	static FbxLoader* sFbxLoader_;

	// D3D12デバイス（借りて来るものの入れ物）
	ID3D12Device* device_ = nullptr;

	// 設定の状態を表すフラグ
	const uint32_t flag_ =
		(uint32_t)aiProcess_Triangulate | //三角面化
		(uint32_t)aiProcess_CalcTangentSpace | //接線ベクトル生成
		(uint32_t)aiProcess_GenSmoothNormals | //スムージングベクトル生成
		(uint32_t)aiProcess_GenUVCoords | //非マッピングを適切なUV座標に変換
		(uint32_t)aiProcess_RemoveRedundantMaterials | //冗長なマテリアルを削除
		(uint32_t)aiProcess_OptimizeMeshes | //メッシュ数を最適化
		(uint32_t)aiProcess_MakeLeftHanded | //ノードを左手座標系に
		(uint32_t)aiProcess_GenBoundingBoxes | //AABBを生成
		(uint32_t)aiProcess_JoinIdenticalVertices |//インデックスを生成
		(uint32_t)aiProcess_LimitBoneWeights;//各頂点が影響を受けるボーンを4に制限

	// シーン
	const aiScene* mScene_;

	// テクスチャ
	Texture textureHandle_;
};

