#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>
#include <DirectXTex.h>
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>

#include "Quaternion.h"
#include "Texture.h"
#include <Mesh.h>
#include "Material.h"
#include <LightGroup.h>
#include "WorldTransform.h"
#include <ViewProjection.h>
#include <assimp/anim.h>
#include "ShaderObj.h"

class FbxModel
{
public:
	// フレンドクラス
	friend class FbxLoader;

public://定数
	static const int MAX_BONE_INDICES = 4;


public: // サブクラス
	// 頂点データ構造体
	struct VertexPosNormalUvFbxSkin
	{
		DirectX::XMFLOAT3 pos; // xyz座標
		DirectX::XMFLOAT3 normal; // 法線ベクトル
		DirectX::XMFLOAT2 uv;  // uv座標

		uint32_t boneIndex[MAX_BONE_INDICES];
		float boneWeight[MAX_BONE_INDICES];
	};
	// 定数バッファ用データ構造体
	struct ConstBufferPolygonExplosion
	{
		float Destruction = 0.0f;
		float ScaleFactor = 1.0f;
		float RotationFactor = 0.0f;
		float PositionFactor = 0.0f;
	};

private:
	// Microsoft::WRL::を省略
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;


public:

	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		Vector4 color;	// 色 (RGBA)
		Matrix4 mat;	// ３Ｄ変換行列
	};

	//1メッシュに持てるボーンの最大個数
	static const int MAX_BONES = 128;

	//定数バッファ用データ構造体
	struct ConstBufferDataSkin {
		Matrix4 bones[MAX_BONES];
	};

	//アニメーションがない場合
	struct ConstBufferDataInitialMatrix {
		Matrix4 InitialMatrix;
	};


	//定数バッファ(スキン)
	static Microsoft::WRL::ComPtr<ID3D12Resource> constBuffSkin_;
	//定数バッファ(アニメーションなし)
	static Microsoft::WRL::ComPtr<ID3D12Resource> constBuffNothing_;

private:
	static const std::string kBaseDirectory_;
	static const std::string kDefaultModelName_;

private: // 静的メンバ変数
	// デスクリプタサイズ
	static uint32_t sDescriptorHandleIncrementSize_;
	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList_;
	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;
	// ライト
	static std::unique_ptr<LightGroup> lightGroup_;
	// 借りてくるデバイス
	static ID3D12Device* device_;

	static ShaderObj* sVsShader_;// 頂点シェーダー
	static ShaderObj* sPsShader_;// ピクセルシェーダー
	static ShaderObj* sGsShader_;// ジオメトリシェーダー

public: // 静的メンバ関数


	// 静的初期化
	static void StaticInitialize();

	// 静的終了処理
	static void StaticFainalize();

	// グラフィックスパイプラインの初期化
	static void InitializeGraphicsPipeline();

	// 3Dモデル生成
	static FbxModel* Create();

	// OBJファイルからメッシュ生成
	static FbxModel* CreateFromFbx(const std::string& modelname, bool smoothing = false);

	// 借りてくるデバイスをセット
	static void SetDevice(ID3D12Device* device) { device_ = device; }

	// 描画前処理
	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	// 描画後処理
	static void PostDraw();

public: // メンバ関数
	// デストラクタ
	~FbxModel();

	// 初期化
	void Initialize();

	void FbxUpdate(float frem);


	// 描画
	//void Draw(
	//	const WorldTransform& worldTransform, const ViewProjection& viewProjection);

	void Draw(WorldTransform* worldTransform, ViewProjection* viewProjection);

	void Draw(WorldTransform* worldTransform, ViewProjection* viewProjection, Texture textureHadle);


	void ModelAnimation(float frame, aiAnimation* Animation, int BoneNum);

	void ReadNodeHeirarchy(Mesh* mesh, aiAnimation* Animation, FLOAT AnimationTime, Node* pNode, Matrix4& mxIdentity);

	aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string& strNodeName);

	void CalcInterpolatedScaling(Vector3& mxOut, float AnimationTime, const aiNodeAnim* pNodeAnim);

	bool FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, uint32_t& nScalingIndex);

	void CalcInterpolatedRotation(Vector4& mxOut, float AnimationTime, const aiNodeAnim* pNodeAnim);

	bool FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, uint32_t& nRotationIndex);

	void CalcInterpolatedPosition(Vector3& mxOut, float AnimationTime, const aiNodeAnim* pNodeAnim);

	bool FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, uint32_t& nPosIndex);

	const ConstBufferPolygonExplosion GetPolygonExplosion() { return *constMap_; }

	const void SetPolygonExplosion(ConstBufferPolygonExplosion polygonExplosion) { *constMap_ = polygonExplosion; }

	// メッシュコンテナを取得
	inline const std::vector<Mesh*>& GetMeshes() { return meshes_; }

	Matrix4 GetLeftBonePos();
	Matrix4 GetRightBonePos();

	void SetTextureHandle(Texture textureHandle) { modelTextureHandle_ = textureHandle; }

	

private:

	// ノード配列
	std::vector<Node> nodes_;

	Matrix4 globalInverseTransform_;

	// 名前
	std::string name_;
	// メッシュコンテナ
	std::vector<Mesh*> meshes_;
	// マテリアルコンテナ
	std::unordered_map<std::string, Material*> materials_;
	// デフォルトマテリアル
	Material* defaultMaterial_ = nullptr;

	Texture modelTextureHandle_;

	Matrix4 matrixL_;
	Matrix4 matrixR_;

	WorldTransform naosi_;

	//定数バッファ
	ComPtr<ID3D12Resource> constBuff_;

	ConstBufferPolygonExplosion* constMap_;

};



