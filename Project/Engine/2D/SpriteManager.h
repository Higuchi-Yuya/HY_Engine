#pragma once

#include "DirectXCommon.h"
#include "Texture.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <TextureManager.h>
#include "ShaderObj.h"

class SpriteManager
{
public:// サブクラス
	// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		Vector4 color;//色(RGBA)
		Matrix4 mat;// 3D変換行列
	};

	// 頂点データ構造体
	struct Vertex
	{
		Vector3 pos; // xyz座標
		Vector2 uv;  // uv座標
	};

public:// メンバ関数
	// 初期化
	void Initialize(DirectXCommon* dxcommon);

	// 描画前設定
	void PreDraw();

	// 描画後設定
	void PostDraw();

	// テクスチャに必要なコマンド設定
	void SetTextureCommands(Texture* index);

	// パイプラインのアクセス
	ID3D12PipelineState *GetPipelineState() { return pipelineState_.Get(); }

	// ルートシグネチャのアクセス
	ID3D12RootSignature* GetRootSignature() { return rootSignature_.Get(); }

public:
	static DirectXCommon* sDxcommon_;
private:
	
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;		//パイプラインステート
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;		//ルートシグネチャ

	ShaderObj vsShader_;// 頂点シェーダー
	ShaderObj psShader_;// ピクセルシェーダー

	// 結果確認
	HRESULT result = 0;
};

