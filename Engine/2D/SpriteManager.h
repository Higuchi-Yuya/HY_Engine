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
#pragma comment(lib, "d3dcompiler.lib")

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
	void SetTextureCommands(uint32_t index);

	//指定番号のテクスチャバッファを取得
	ID3D12Resource* GetTextureBuffer(uint32_t index)const { return Texture::textureBuffers_[index].Get(); }

public:
	static DirectXCommon* dxcommon_;
private:
	
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;		//パイプラインステート
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;		//ルートシグネチャ

	// 結果確認
	HRESULT result = 0;
};

