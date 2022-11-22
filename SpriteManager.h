#pragma once
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

class SpriteManager
{
public:// メンバ関数
	// 初期化
	void Initialize(DirectXCommon* dxcommon);

	// 描画
	void Draw();

public:
	static DirectXCommon* dxcommon_;
private:

	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;		//パイプラインステート
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;		//ルートシグネチャ
	// 結果確認
	HRESULT result;
};

