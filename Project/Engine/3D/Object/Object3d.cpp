#include "Object3d.h"
#include <d3dcompiler.h>
#include "BaseCollider.h"
#pragma comment(lib, "d3dcompiler.lib")
#include "CollisionManager.h"

//using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
ID3D12Device* Object3d::sDevice_ = nullptr;
LightGroup* Object3d::sLight_ = nullptr;
Fog* Object3d::sFog_ = nullptr;

ID3D12GraphicsCommandList* Object3d::sCmdList_ = nullptr;
ComPtr<ID3D12RootSignature> Object3d::sRootsignature_;
ComPtr<ID3D12PipelineState> Object3d::sPipelinestateNormal_ = nullptr;
ComPtr<ID3D12PipelineState> Object3d::sPipelinestateADDITION_ = nullptr;
ComPtr<ID3D12PipelineState> Object3d::sPipelinestateADDITIONALPHA_ = nullptr;
ComPtr<ID3D12PipelineState> Object3d::sPipelinestateSUBTRACTION_ = nullptr;
ComPtr<ID3D12PipelineState> Object3d::sPipelinestateSCREEN_ = nullptr;
ComPtr<ID3D12PipelineState> Object3d::sPipelinestateSilhouette_ = nullptr;
ComPtr<ID3D12PipelineState> Object3d::sPipelinestateTransParent_ = nullptr;
ComPtr<ID3D12PipelineState> Object3d::sPipelinestateShield_ = nullptr;
ComPtr<ID3D12PipelineState> Object3d::sPipelinestateLightAtten_ = nullptr;

std::vector<D3D12_INPUT_ELEMENT_DESC> Object3d::sInputLayout_;

ComPtr<ID3DBlob> Object3d::sVsBlob_;
ComPtr<ID3DBlob> Object3d::sPsBlob_;	
ComPtr<ID3DBlob> Object3d::sErrorBlob_;

ShaderObj* Object3d::sVsShader_ = nullptr;
ShaderObj* Object3d::sPsShader_ = nullptr;

ShaderObj* Object3d::sLightAttenVsShader_ = nullptr;
ShaderObj* Object3d::sLightAttenPsShader_ = nullptr;

ShaderObj* Object3d::sSilhouetteVsShader_ = nullptr;
ShaderObj* Object3d::sSilhouettePsShader_ = nullptr;
//ComPtr<ID3DBlob> Object3d::sRootSigBlob_;
//Object3d::BlendMode Object3d::blendMode = BlendMode::NORMAL;

void Object3d::StaticInitialize(ID3D12Device* device)
{
	// nullptrチェック
	assert(device);

	Object3d::sDevice_ = device;

	// モデルのデバイスを生成
	Model::SetDevice(sDevice_);

	// ワールドトランスフォームにデバイスを貸す
	WorldTransform::StaticInitialize(sDevice_);

	Dissolve::StaticInitialize(sDevice_);

	// シェーダーファイルの読み込みと初期化
	InitializeShader();

	// ルートシグネチャの初期化
	InitializeRootSignature();

	// シルエットのシェーダーオブジェクトの初期化
	InitializeShaderSilhouette();

	// ライトの減衰用のシェーダーオブジェクトの初期化
	InitShaderLightAtten();

	// パイプライン初期化

	// ノーマル
	InitializeGraphicsPipelineNormal();

	//// 加算
	//InitializeGraphicsPipelineADDITION();

	//// 加算（透過あり）
	//InitializeGraphicsPipelineADDITIONALPHA();

	//// 減算
	//InitializeGraphicsPipelineSUBTRACTION();

	//// スクリーン
	//InitializeGraphicsPipelineSCREEN();

	// シルエット
	InitializeGraphicsPipelineSilhouette();

	// 窓透過用(ステンシル読み込み側)
	InitializeGraphicsPipelineTransParent();

	// 窓遮蔽物用(ステンシル書き込み側)
	InitializeGraphicsPipelineShield();

	// ライト減衰用のパイプラインの初期化
	InitGraphicsPipelineLightAtten();
}

void Object3d::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	//assert(Object3d::sCmdList_ == nullptr);

	// コマンドリストをセット
	Object3d::sCmdList_ = cmdList;

	// パイプラインステートの設定
	sCmdList_->SetPipelineState(sPipelinestateNormal_.Get());
	// ルートシグネチャの設定
	sCmdList_->SetGraphicsRootSignature(sRootsignature_.Get());
	// プリミティブ形状を設定
	sCmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Object3d::PostDraw()
{
	// コマンドリストを解除
	Object3d::sCmdList_ = nullptr;
}

Object3d* Object3d::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	Object3d* object3d = new Object3d();
	if (object3d == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!object3d->Initialize()) {
		delete object3d;
		assert(0);
		return nullptr;
	}

	return object3d;
}

void Object3d::InitializeGraphicsPipelineNormal()
{
	HRESULT result = S_FALSE;

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = *sVsShader_->GetShader();//CD3DX12_SHADER_BYTECODE(sVsBlob_.Get());
	gpipeline.PS = *sPsShader_->GetShader();//CD3DX12_SHADER_BYTECODE(sPsBlob_.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	//gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
	
	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;


	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = sInputLayout_.data();
	gpipeline.InputLayout.NumElements = (UINT)sInputLayout_.size();

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	gpipeline.pRootSignature = sRootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = sDevice_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sPipelinestateNormal_));
	assert(SUCCEEDED(result));

}

void Object3d::InitializeGraphicsPipelineADDITION()
{
	HRESULT result = S_FALSE;

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(sVsBlob_.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(sPsBlob_.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.SrcBlend = D3D12_BLEND_ONE;
	blenddesc.DestBlend = D3D12_BLEND_ONE;

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;


	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = sInputLayout_.data();
	gpipeline.InputLayout.NumElements = (UINT)sInputLayout_.size();

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	gpipeline.pRootSignature = sRootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = sDevice_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sPipelinestateADDITION_));
	assert(SUCCEEDED(result));
}

void Object3d::InitializeGraphicsPipelineADDITIONALPHA()
{
	HRESULT result = S_FALSE;

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(sVsBlob_.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(sPsBlob_.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_ONE;

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;


	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = sInputLayout_.data();
	gpipeline.InputLayout.NumElements = (UINT)sInputLayout_.size();

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	gpipeline.pRootSignature = sRootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = sDevice_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sPipelinestateADDITIONALPHA_));
	assert(SUCCEEDED(result));
}

void Object3d::InitializeGraphicsPipelineSUBTRACTION()
{
	HRESULT result = S_FALSE;

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(sVsBlob_.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(sPsBlob_.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.SrcBlend = D3D12_BLEND_ZERO;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_COLOR;

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;


	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = sInputLayout_.data();
	gpipeline.InputLayout.NumElements = (UINT)sInputLayout_.size();

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	gpipeline.pRootSignature = sRootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = sDevice_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sPipelinestateSUBTRACTION_));
	assert(SUCCEEDED(result));
}

void Object3d::InitializeGraphicsPipelineSCREEN()
{
	HRESULT result = S_FALSE;

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(sVsBlob_.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(sPsBlob_.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blenddesc.DestBlend = D3D12_BLEND_ONE;

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;


	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = sInputLayout_.data();
	gpipeline.InputLayout.NumElements = (UINT)sInputLayout_.size();

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	gpipeline.pRootSignature = sRootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = sDevice_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sPipelinestateSCREEN_));
	assert(SUCCEEDED(result));
}

void Object3d::InitializeGraphicsPipelineSilhouette()
{
	HRESULT result = S_FALSE;

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = *sSilhouetteVsShader_->GetShader();//CD3DX12_SHADER_BYTECODE(sVsBlob_.Get());
	gpipeline.PS = *sSilhouettePsShader_->GetShader();//CD3DX12_SHADER_BYTECODE(sPsBlob_.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = sInputLayout_.data();
	gpipeline.InputLayout.NumElements = (UINT)sInputLayout_.size();

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	//gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	gpipeline.pRootSignature = sRootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = sDevice_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sPipelinestateSilhouette_));
	assert(SUCCEEDED(result));
}

void Object3d::InitializeGraphicsPipelineTransParent()
{
	HRESULT result = S_FALSE;

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = *sVsShader_->GetShader();//CD3DX12_SHADER_BYTECODE(sVsBlob_.Get());
	gpipeline.PS = *sPsShader_->GetShader();//CD3DX12_SHADER_BYTECODE(sPsBlob_.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	
	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.StencilEnable = true;
	gpipeline.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	gpipeline.DepthStencilState.StencilWriteMask = 0x00;
	gpipeline.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_ZERO;
	gpipeline.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	gpipeline.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	gpipeline.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	gpipeline.DepthStencilState.BackFace.StencilFailOp = gpipeline.DepthStencilState.FrontFace.StencilFailOp;
	gpipeline.DepthStencilState.BackFace.StencilDepthFailOp = gpipeline.DepthStencilState.FrontFace.StencilDepthFailOp;
	gpipeline.DepthStencilState.BackFace.StencilPassOp = gpipeline.DepthStencilState.FrontFace.StencilPassOp;
	gpipeline.DepthStencilState.BackFace.StencilFunc = gpipeline.DepthStencilState.FrontFace.StencilFunc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = sInputLayout_.data();
	gpipeline.InputLayout.NumElements = (UINT)sInputLayout_.size();

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	gpipeline.pRootSignature = sRootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = sDevice_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sPipelinestateTransParent_));
	assert(SUCCEEDED(result));
}

void Object3d::InitializeGraphicsPipelineShield()
{
	HRESULT result = S_FALSE;

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = *sVsShader_->GetShader();//CD3DX12_SHADER_BYTECODE(sVsBlob_.Get());
	gpipeline.PS = *sPsShader_->GetShader();//CD3DX12_SHADER_BYTECODE(sPsBlob_.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	//gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	//gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;

	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	gpipeline.DepthStencilState.DepthEnable = false;
	gpipeline.DepthStencilState.StencilEnable = true;
	gpipeline.DepthStencilState.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	gpipeline.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	gpipeline.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	gpipeline.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	gpipeline.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	gpipeline.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	gpipeline.DepthStencilState.BackFace.StencilFailOp = gpipeline.DepthStencilState.FrontFace.StencilFailOp;
	gpipeline.DepthStencilState.BackFace.StencilDepthFailOp = gpipeline.DepthStencilState.FrontFace.StencilDepthFailOp;
	gpipeline.DepthStencilState.BackFace.StencilPassOp = gpipeline.DepthStencilState.FrontFace.StencilPassOp;
	gpipeline.DepthStencilState.BackFace.StencilFunc = gpipeline.DepthStencilState.FrontFace.StencilFunc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = sInputLayout_.data();
	gpipeline.InputLayout.NumElements = (UINT)sInputLayout_.size();

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	gpipeline.pRootSignature = sRootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = sDevice_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sPipelinestateShield_));
	assert(SUCCEEDED(result));
}

void Object3d::InitializeShaderSilhouette()
{
	// 頂点シェーダの読み込みとコンパイル
	sSilhouetteVsShader_ = new ShaderObj;
	sSilhouetteVsShader_->Create("Object/SilhouetteVS.hlsl", "main", "vs_5_0", ShaderObj::ShaderType::VS);

	// ピクセルシェーダの読み込みとコンパイル
	sSilhouettePsShader_ = new ShaderObj;
	sSilhouettePsShader_->Create("Object/SilhouettePS.hlsl", "main", "ps_5_0", ShaderObj::ShaderType::PS);
}

void Object3d::InitGraphicsPipelineLightAtten()
{
	HRESULT result = S_FALSE;

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = *sLightAttenVsShader_->GetShader();//CD3DX12_SHADER_BYTECODE(sVsBlob_.Get());
	gpipeline.PS = *sLightAttenPsShader_->GetShader();//CD3DX12_SHADER_BYTECODE(sPsBlob_.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	// デプスステンシルステート
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;


	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = sInputLayout_.data();
	gpipeline.InputLayout.NumElements = (UINT)sInputLayout_.size();

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	gpipeline.pRootSignature = sRootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = sDevice_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sPipelinestateLightAtten_));
	assert(SUCCEEDED(result));
}

void Object3d::InitShaderLightAtten()
{
	// 頂点シェーダの読み込みとコンパイル
	sLightAttenVsShader_ = new ShaderObj;
	sLightAttenVsShader_->Create("Object/ObjLightAttenVS.hlsl", "main", "vs_5_0", ShaderObj::ShaderType::VS);

	// ピクセルシェーダの読み込みとコンパイル
	sLightAttenPsShader_ = new ShaderObj;
	sLightAttenPsShader_->Create("Object/ObjLightAttenPS.hlsl", "main", "ps_5_0", ShaderObj::ShaderType::PS);
}

void Object3d::InitializeRootSignature()
{
	HRESULT result = S_FALSE;

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV0;
	descRangeSRV0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV1;
	descRangeSRV1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);// t1 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootParams[8];
	//テクスチャレジスタ0番 ---テクスチャシェーダーリソースビュー用
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootParams[0].DescriptorTable.pDescriptorRanges = &descRangeSRV0;			//デスクリプタレンジ
	rootParams[0].DescriptorTable.NumDescriptorRanges = 1;						//デスクリプタレンジ数
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//すべてのシェーダから見える

	//テクスチャレジスタ1番 ---ディゾルブテクスチャシェーダーリソースビュー用
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootParams[1].DescriptorTable.pDescriptorRanges = &descRangeSRV1;			//デスクリプタレンジ
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;						//デスクリプタレンジ数
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//すべてのシェーダから見える

	//定数バッファ0番 ---ワールド変換データ用
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//定数バッファビュー
	rootParams[2].Descriptor.ShaderRegister = 0;					//定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//すべてのシェーダから見える

	//定数バッファ1番 ---ビュープロジェクション変換データ用
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[3].Descriptor.ShaderRegister = 1;					//定数バッファ番号
	rootParams[3].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//すべてのシェーダから見える

	//定数バッファ2番 ---マテリアルバッファビュー用
	rootParams[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[4].Descriptor.ShaderRegister = 2;					//定数バッファ番号
	rootParams[4].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//すべてのシェーダから見える

	//定数バッファ3番 ---ライトバッファビュー用
	rootParams[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[5].Descriptor.ShaderRegister = 3;					//定数バッファ番号
	rootParams[5].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//すべてのシェーダから見える

	//定数バッファ4番 ---フォグバッファビュー用
	rootParams[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[6].Descriptor.ShaderRegister = 4;					//定数バッファ番号
	rootParams[6].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//すべてのシェーダから見える

	//定数バッファ5番 ---ディゾルブバッファビュー用
	rootParams[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[7].Descriptor.ShaderRegister = 5;					//定数バッファ番号
	rootParams[7].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//すべてのシェーダから見える

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootParams), rootParams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* sRootSigBlob_;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &sRootSigBlob_, &sErrorBlob_);
	// ルートシグネチャの生成
	result = sDevice_->CreateRootSignature(0, sRootSigBlob_->GetBufferPointer(), sRootSigBlob_->GetBufferSize(), IID_PPV_ARGS(&sRootsignature_));
	assert(SUCCEEDED(result));
}

void Object3d::InitializeShader()
{

	// 頂点シェーダの読み込みとコンパイル
	sVsShader_ = new ShaderObj;
	sVsShader_->Create("Object/OBJVertexShader.hlsl", "main", "vs_5_0", ShaderObj::ShaderType::VS);

	// ピクセルシェーダの読み込みとコンパイル
	sPsShader_ = new ShaderObj;
	sPsShader_->Create("Object/OBJPixcelShader.hlsl", "main", "ps_5_0", ShaderObj::ShaderType::PS);

	// 頂点レイアウト
	sInputLayout_.push_back
	({
		// xy座標(1行で書いたほうが見やすい)
		   "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		   D3D12_APPEND_ALIGNED_ELEMENT,
		   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	});
	sInputLayout_.push_back
	({
		// 法線ベクトル(1行で書いたほうが見やすい)
		   "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		   D3D12_APPEND_ALIGNED_ELEMENT,
		   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0

	});
	sInputLayout_.push_back
	({
		// uv座標(1行で書いたほうが見やすい)
		   "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		   D3D12_APPEND_ALIGNED_ELEMENT,
		   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	});
}

Object3d::~Object3d()
{
	if (collider_) {
		// 子リジョンマネージャから登録を解除する
		CollisionManager::GetInstance()->RemoveCollider(collider_);
		delete collider_;
	}

}

bool Object3d::Initialize()
{
	// クラス名の文字列を取得
	name_ = typeid(*this).name();

	// nullptrチェック
	worldTransform_.Initialize();

	// フォグの初期化
	sFog_->Initialize();

	// ディゾルブの初期化
	dissolve_.Initialize();

	return true;
}

void Object3d::Update()
{
	
	// ワールドトランスフォームの行列更新と転送
	UpdateWorldMatrix();
	sFog_->UpdateMatrix();

	dissolve_.UpdateMatrix();

	// 末尾に当たり判定更新
	if (collider_) {
		collider_->Update(worldTransform_.matWorld_);
	}
}

void Object3d::UpdateWorldMatrix()
{
	worldTransform_.UpdateMatrix();
}

void Object3d::Draw(ViewProjection* viewProjection)
{
	// nullptrチェック
	assert(sDevice_);
	assert(Object3d::sCmdList_);

	// モデルがセットされていなければ描画をスキップ
	if (model_ == nullptr)return;

	// ワールド変換データ定数バッファビューをセット
	sCmdList_->SetGraphicsRootConstantBufferView(static_cast<uint32_t>(rootParameterIndex::WORLDTRANS),
												 worldTransform_.GetBuff()->GetGPUVirtualAddress());

	// ビュープロジェクション変換データ定数バッファビューをセット
	sCmdList_->SetGraphicsRootConstantBufferView(static_cast<uint32_t>(rootParameterIndex::VIEWPROJECTION), 
												 viewProjection->GetBuff()->GetGPUVirtualAddress());

	// フォグの描画
	sFog_->Draw(sCmdList_, static_cast<uint32_t>(rootParameterIndex::FOGDATA));

	// ライトの描画
	sLight_->Draw(sCmdList_, static_cast<uint32_t>(rootParameterIndex::LIGHTDATA));

	// ディゾルブの描画
	dissolve_.Draw(sCmdList_, static_cast<uint32_t>(rootParameterIndex::DISSOLVEDATA));

	// モデルを描画
	model_->Draw(sCmdList_, (uint32_t)rootParameterIndex::MATERIALDATA, (uint32_t)rootParameterIndex::BODYTEXTURE);

}

void Object3d::SetModel(Model* model)
{
	model_ = model; 
	worldTransform_.minVertex_ = model->GetMinVertex();
	worldTransform_.maxVertex_ = model->GetMaxVertex();
}

void Object3d::SetCollider(BaseCollider* collider)
{
	//collider_->SetObject(this);
	collider_ = collider;

	// ワールド行列を更新しおく
	worldTransform_.UpdateMatrix();

	// コリジョンマネージャに登録
	CollisionManager::GetInstance()->AddCollider(collider_);

	

	// コライダーを更新しておく
	collider_->Update(worldTransform_.matWorld_);

}

void Object3d::SetIsStencil(bool IsStencil)
{
	IsStencilTest_ = IsStencil;
	if (IsStencilTest_ == true) {
		worldTransform_.color.w = 0;
		Update();
	}
}

void Object3d::SetBlendMode(BlendMode mode)
{
	switch (mode)
	{
	case Object3d::NORMAL:// ノーマルブレンド
		sCmdList_->SetPipelineState(sPipelinestateNormal_.Get());
		break;
	case Object3d::ADDITION:// 加算ブレンド
		sCmdList_->SetPipelineState(sPipelinestateADDITION_.Get());
		break;
	case Object3d::ADDITIONALPHA:// 加算ブレンド（透過あり）
		sCmdList_->SetPipelineState(sPipelinestateADDITIONALPHA_.Get());
		break;
	case Object3d::SUBTRACTION:// 減算ブレンド
		sCmdList_->SetPipelineState(sPipelinestateSUBTRACTION_.Get());
		break;
	case Object3d::SCREEN:// スクリーン
		sCmdList_->SetPipelineState(sPipelinestateSCREEN_.Get());
		break;
	case Object3d::SILHOUETTE:// シルエット
		sCmdList_->SetPipelineState(sPipelinestateSilhouette_.Get());
		break;
	case Object3d::TransParent:// 窓透過用
		sCmdList_->SetPipelineState(sPipelinestateTransParent_.Get());
		break;
	case Object3d::Shield:// 窓透過用
		sCmdList_->SetPipelineState(sPipelinestateShield_.Get());
		break;

	case Object3d::LightAtten:// 光の減衰用
		sCmdList_->SetPipelineState(sPipelinestateLightAtten_.Get());
		break;
	default:
		break;
	}
}

void Object3d::StaticFinalize()
{
	sRootsignature_ = nullptr;
	sPipelinestateNormal_ = nullptr;
	sPipelinestateADDITION_ = nullptr;
	sPipelinestateADDITIONALPHA_ = nullptr;
	sPipelinestateSUBTRACTION_ = nullptr;
	sPipelinestateSCREEN_ = nullptr;
	sPipelinestateSilhouette_ = nullptr;
	sPipelinestateTransParent_ = nullptr;
	sPipelinestateShield_ = nullptr;
	sPipelinestateLightAtten_ = nullptr;

	sInputLayout_.clear();
}
