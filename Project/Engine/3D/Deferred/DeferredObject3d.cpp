#include "DeferredObject3d.h"
#include <PostEffectHandleManager.h>
using namespace Microsoft::WRL;

ID3D12Device* DeferredObject3d::sDevice_ = nullptr;
ID3D12GraphicsCommandList* DeferredObject3d::sCmdList_ = nullptr;

ComPtr<ID3D12RootSignature> DeferredObject3d::sRootsignature_ = nullptr;
ComPtr<ID3D12PipelineState> DeferredObject3d::sPipelinestateNormal_ = nullptr;
std::vector<D3D12_INPUT_ELEMENT_DESC>  DeferredObject3d::sInputLayout_;

ComPtr<ID3DBlob> DeferredObject3d::sVsBlob_;
ComPtr<ID3DBlob> DeferredObject3d::sPsBlob_;
ComPtr<ID3DBlob> DeferredObject3d::sErrorBlob_;

ShaderObj* DeferredObject3d::sVsShader_ = nullptr;
ShaderObj* DeferredObject3d::sPsShader_ = nullptr;

Microsoft::WRL::ComPtr<ID3D12Resource> DeferredObject3d::texBuff_[6];
Handles DeferredObject3d::handles_[6];

SpriteManager::Vertex DeferredObject3d::vertices_[kVertNum_] = {
	{{-1.0f,-1.0f, 0.0f },{0.0f,1.0f}}, // 左下
	{{-1.0f,+1.0f, 0.0f },{0.0f,0.0f}}, // 左上
	{{+1.0f,-1.0f, 0.0f },{1.0f,1.0f}}, // 右下
	{{+1.0f,+1.0f, 0.0f },{1.0f,0.0f}}, // 右上
};

SpriteManager::Vertex* DeferredObject3d::vertMap_;

D3D12_VERTEX_BUFFER_VIEW DeferredObject3d::vbView_;
Microsoft::WRL::ComPtr<ID3D12Resource> DeferredObject3d::vertBuff_;
Microsoft::WRL::ComPtr <ID3D12Resource>DeferredObject3d::depthBuff_;

const float DeferredObject3d::clearColor_[4] = { 0.0f,0.0f,0.0f,0.0f };
HRESULT DeferredObject3d::result;

void DeferredObject3d::StaticInitialize(ID3D12Device* device)
{
	// nullptrチェック
	assert(device);

	DeferredObject3d::sDevice_ = device;

	// レンダーテクスチャ関連の初期化
	CreateVertBuff();

	CreateTex();

	CreateSRV();

	CreateRTV();

	CreateDepthBuff();

	CreateDSV();

	// シェーダーファイルの読み込みと初期化
	InitializeShader();

	// ルートシグネチャの初期化
	InitializeRootSignature();

	// パイプライン初期化

	// ノーマル
	InitializeGraphicsPipelineNormal();
}

void DeferredObject3d::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// コマンドリストをセット
	DeferredObject3d::sCmdList_ = cmdList;

	for (int i = 0; i < 6; i++) {
		// リソースバリアを作成
		CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_[i].Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

		// リソースバリアを変更（シェーダリソース）描画可能
		sCmdList_->ResourceBarrier(1, &resourceBarrier);
	}
	// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH[6];
	for (int i = 0; i < 6; i++) {
		rtvH[i] = handles_[i].rtvCpuHandle_;
	}

	// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		handles_[0].dsvCpuHandle_;

	// レンダーターゲットをセット
	sCmdList_->OMSetRenderTargets(6, rtvH, false, &dsvH);

	CD3DX12_VIEWPORT viewPort[6];
	CD3DX12_RECT rect[6];

	for (size_t i = 0; i < 6; i++)
	{
		// ビューポートの設定
		viewPort[i] = CD3DX12_VIEWPORT(0.0f, 0.0f, WinApp::window_width, WinApp::window_height);
		// シザリング矩形の設定
		rect[i] = CD3DX12_RECT(0, 0, WinApp::window_width, WinApp::window_height);
	}
	sCmdList_->RSSetViewports(6, viewPort);
	sCmdList_->RSSetScissorRects(6, rect);

	// 全画面クリア
	for (size_t i = 0; i < 6; i++)
	{
		sCmdList_->ClearRenderTargetView(rtvH[i], clearColor_, 0, nullptr);
	}

	// 深度バッファのクリア
	sCmdList_->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DeferredObject3d::PostDraw()
{
	for (int i = 0; i < 6; i++) {
		// リソースバリアを作成
		CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_[i].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

		// リソースバリアを変更（描画可能シェーダーリソース）
		sCmdList_->ResourceBarrier(1, &resourceBarrier);
	}
}

void DeferredObject3d::DrawDeferredObjRenderTex()
{
	// パイプラインステートの設定
	// パイプラインステートとルートシグネチャの設定コマンド
	sCmdList_->SetPipelineState(sPipelinestateNormal_.Get());
	sCmdList_->SetGraphicsRootSignature(sRootsignature_.Get());

	// プリミティブ形状の設定コマンド
	sCmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // 三角形リスト

	// SRVヒープ
	ID3D12DescriptorHeap* ppHeaps[] = { PostRenderBase::GetInstance()->GetSrvDesc().Get() };
	sCmdList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// SRVヒープの先頭にあるSRVをルートパラメータ0番に設定
	sCmdList_->SetGraphicsRootDescriptorTable(0, handles_[0].srvGpuHandle_);

	// 頂点バッファビューの設定コマンド
	sCmdList_->IASetVertexBuffers(0, 1, &vbView_);

	// 描画コマンド
	sCmdList_->DrawInstanced(_countof(vertices_), 1, 0, 0); // 全ての頂点を使って描画
}

DeferredObject3d* DeferredObject3d::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	DeferredObject3d* object3d = new DeferredObject3d();
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

void DeferredObject3d::InitializeRootSignature()
{

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV0;
	descRangeSRV0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootParams[4];
	//テクスチャレジスタ0番 ---テクスチャシェーダーリソースビュー用
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootParams[0].DescriptorTable.pDescriptorRanges = &descRangeSRV0;			//デスクリプタレンジ
	rootParams[0].DescriptorTable.NumDescriptorRanges = 1;						//デスクリプタレンジ数
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//すべてのシェーダから見える

	//定数バッファ0番 ---ワールド変換データ用
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//定数バッファビュー
	rootParams[1].Descriptor.ShaderRegister = 0;					//定数バッファ番号
	rootParams[1].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//すべてのシェーダから見える

	//定数バッファ1番 ---ビュープロジェクション変換データ用
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[2].Descriptor.ShaderRegister = 1;					//定数バッファ番号
	rootParams[2].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//すべてのシェーダから見える

	//定数バッファ2番 ---マテリアルバッファビュー用
	rootParams[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	//種類
	rootParams[3].Descriptor.ShaderRegister = 2;					//定数バッファ番号
	rootParams[3].Descriptor.RegisterSpace = 0;						//デフォルト値
	rootParams[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	//すべてのシェーダから見える

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

void DeferredObject3d::InitializeShader()
{
	// 頂点シェーダの読み込みとコンパイル
	sVsShader_ = new ShaderObj;
	sVsShader_->Create("Deferred/DeferredObjVS.hlsl", "main", "vs_5_0", ShaderObj::ShaderType::VS);

	// ピクセルシェーダの読み込みとコンパイル
	sPsShader_ = new ShaderObj;
	sPsShader_->Create("Deferred/DeferredObjPS.hlsl", "main", "ps_5_0", ShaderObj::ShaderType::PS);

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

void DeferredObject3d::InitializeGraphicsPipelineNormal()
{

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = *sVsShader_->GetShader();//CD3DX12_SHADER_BYTECODE(sVsBlob_.Get());
	gpipeline.PS = *sPsShader_->GetShader();//CD3DX12_SHADER_BYTECODE(sPsBlob_.Get());

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
	gpipeline.BlendState.RenderTarget[1] = blenddesc;
	gpipeline.BlendState.RenderTarget[2] = blenddesc;
	gpipeline.BlendState.RenderTarget[3] = blenddesc;
	gpipeline.BlendState.RenderTarget[4] = blenddesc;
	gpipeline.BlendState.RenderTarget[5] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = sInputLayout_.data();
	gpipeline.InputLayout.NumElements = (UINT)sInputLayout_.size();

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 6;	// 描画対象は6つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.RTVFormats[2] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.RTVFormats[3] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.RTVFormats[4] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.RTVFormats[5] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA

	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	gpipeline.pRootSignature = sRootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = sDevice_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sPipelinestateNormal_));
	assert(SUCCEEDED(result));
}

void DeferredObject3d::CreateVertBuff()
{
	// 頂点バッファの設定
	// ヒープ
	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// テクスチャリソース設定
	CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(SpriteManager::Vertex) * kVertNum_);

	// 頂点バッファ生成
	result = sDevice_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_));

	assert(SUCCEEDED(result));

	// 頂点バッファへのデータ転送
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);

	if (SUCCEEDED(result)) {
		memcpy(vertMap_, vertices_, sizeof(vertices_));
		vertBuff_->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeof(SpriteManager::Vertex) * 4;
	vbView_.StrideInBytes = sizeof(SpriteManager::Vertex);
}

void DeferredObject3d::CreateTex()
{
	// テクスチャリソース設定
	CD3DX12_RESOURCE_DESC texresDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		WinApp::window_width,
		static_cast<uint32_t>(WinApp::window_height),
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	// ヒープ
	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// テクスチャバリュー
	CD3DX12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor_);

	// テクスチャバッファの生成
	for (int i = 0; i < 6; i++) {
		result = sDevice_->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&texresDesc,
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
			&clearValue,
			IID_PPV_ARGS(&texBuff_[i]));
	}
	assert(SUCCEEDED(result));
}

void DeferredObject3d::CreateSRV()
{
	for (int i = 0; i < 6; i++) {
		PostRenderBase::GetInstance()->CreateSRV(texBuff_[i].Get(), handles_[i].srvCpuHandle_, handles_[i].srvGpuHandle_);
	}
}

void DeferredObject3d::CreateRTV()
{
	for (int i = 0; i < 6; i++) {
		PostRenderBase::GetInstance()->CreateRTV(texBuff_[i].Get(), handles_[i].rtvCpuHandle_);
	}
}

void DeferredObject3d::CreateDepthBuff()
{
	// 深度バッファリソース設定
	CD3DX12_RESOURCE_DESC depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
		WinApp::window_width,
		WinApp::window_height,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	// テクスチャバリュー
	CD3DX12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT_S8X24_UINT, 1.0f, 0);

	// 深度バッファの生成
	result = sDevice_->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&clearValue,
		IID_PPV_ARGS(&depthBuff_)
	);
	assert(SUCCEEDED(result));
}

void DeferredObject3d::CreateDSV()
{
	PostRenderBase::GetInstance()->CreateDSV(depthBuff_.Get(), handles_[0].dsvCpuHandle_, DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
}

void DeferredObject3d::StaticFinalize()
{
	sRootsignature_ = nullptr;
	sPipelinestateNormal_ = nullptr;
	vertBuff_ = nullptr;
	depthBuff_ = nullptr;
	for (size_t i = 0; i < 6; i++)
	{
		texBuff_[i] = nullptr;
	}

	sInputLayout_.clear();
}

DeferredObject3d::~DeferredObject3d()
{

}

bool DeferredObject3d::Initialize()
{
	// クラス名の文字列を取得
	name_ = typeid(*this).name();

	// nullptrチェック
	worldTransform_.Initialize();

	return true;
}

void DeferredObject3d::Update()
{
	// ワールドトランスフォームの行列更新と転送
	UpdateWorldMatrix();
}

void DeferredObject3d::UpdateWorldMatrix()
{
	worldTransform_.UpdateMatrix();
}

void DeferredObject3d::Draw(ViewProjection* viewProjection)
{
	// nullptrチェック
	assert(sDevice_);
	assert(DeferredObject3d::sCmdList_);

	// モデルがセットされていなければ描画をスキップ
	if (model_ == nullptr)return;

	// パイプラインステートの設定
	// パイプラインステートとルートシグネチャの設定コマンド
	sCmdList_->SetPipelineState(sPipelinestateNormal_.Get());
	sCmdList_->SetGraphicsRootSignature(sRootsignature_.Get());

	// プリミティブ形状の設定コマンド
	sCmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // 三角形リスト

	// ワールド変換データ定数バッファビューをセット
	sCmdList_->SetGraphicsRootConstantBufferView((uint32_t)rootParameterIndex::WORLDTRANS,
		worldTransform_.GetBuff()->GetGPUVirtualAddress());

	// ビュープロジェクション変換データ定数バッファビューをセット
	sCmdList_->SetGraphicsRootConstantBufferView((uint32_t)rootParameterIndex::VIEWPROJECTION,
		viewProjection->GetBuff()->GetGPUVirtualAddress());

	// モデルを描画
	model_->Draw(sCmdList_, (uint32_t)rootParameterIndex::MATERIALDATA, (uint32_t)rootParameterIndex::BODYTEXTURE);
}

void DeferredObject3d::SetModel(Model* model)
{
	model_ = model;
	worldTransform_.minVertex_ = model->GetMinVertex();
	worldTransform_.maxVertex_ = model->GetMaxVertex();
}
