#include "DeferredRender.h"
#include <PostEffectHandleManager.h>
ID3D12Device* DeferredRender::sDevice_ = nullptr;
ID3D12GraphicsCommandList* DeferredRender::sCmdList_ = nullptr;

const float DeferredRender::clearColor_[4] = { 0.0f,0.0f,0.0f,0.0f };

LightGroup* DeferredRender::sLight_ = nullptr;
Fog* DeferredRender::sFog_ = nullptr;
D3D12_CPU_DESCRIPTOR_HANDLE DeferredRender::sDsvHandle_;

DeferredRender::DeferredRender()
{
}

void DeferredRender::Initialize()
{
	// 初期化

	CreateVertBuff();

	CreateTex();

	CreateSRV();

	CreateRTV();

	CreateDepthBuff();

	CreateDSV();

	CreateGraphicsPipelineState();


}

void DeferredRender::Update()
{
	sFog_->UpdateMatrix();
}

void DeferredRender::ImguiUpdate()
{
}

void DeferredRender::Draw()
{
	// パイプラインステートの設定
	// パイプラインステートとルートシグネチャの設定コマンド
	sCmdList_->SetPipelineState(pipelineState_.Get());
	sCmdList_->SetGraphicsRootSignature(rootSignature_.Get());

	// プリミティブ形状の設定コマンド
	sCmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP); // 三角形リスト

	// SRVヒープ
	ID3D12DescriptorHeap* ppHeaps[] = { PostRenderBase::GetInstance()->GetSrvDesc().Get() };
	sCmdList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// SRVヒープの先頭にあるSRVをルートパラメータ1番に設定
	sCmdList_->SetGraphicsRootDescriptorTable((uint32_t)rootParameterIndex::COLORMAP, PostEffectHandleManager::GetPostEffectHandle("colorMap").srvGpuHandle_);
	sCmdList_->SetGraphicsRootDescriptorTable((uint32_t)rootParameterIndex::WORLDPOSMAP, PostEffectHandleManager::GetPostEffectHandle("worldPosMap").srvGpuHandle_);
	sCmdList_->SetGraphicsRootDescriptorTable((uint32_t)rootParameterIndex::CAMERAPOSMAP, PostEffectHandleManager::GetPostEffectHandle("cameraPosMap").srvGpuHandle_);
	sCmdList_->SetGraphicsRootDescriptorTable((uint32_t)rootParameterIndex::NORMALMAP, PostEffectHandleManager::GetPostEffectHandle("normalMap").srvGpuHandle_);
	sCmdList_->SetGraphicsRootDescriptorTable((uint32_t)rootParameterIndex::AMBIENTMAP, PostEffectHandleManager::GetPostEffectHandle("ambientMap").srvGpuHandle_);
	sCmdList_->SetGraphicsRootDescriptorTable((uint32_t)rootParameterIndex::DIFFUSEMAP, PostEffectHandleManager::GetPostEffectHandle("diffuseMap").srvGpuHandle_);
	sCmdList_->SetGraphicsRootDescriptorTable((uint32_t)rootParameterIndex::SPECULARMAP, PostEffectHandleManager::GetPostEffectHandle("specularMap").srvGpuHandle_);

	// 頂点バッファビューの設定コマンド
	sCmdList_->IASetVertexBuffers(0, 1, &vbView_);

	// 定数バッファビュー(CBV)の設定コマンド
	sLight_->Draw(sCmdList_, (uint32_t)rootParameterIndex::LIGHTDATA);
	sFog_->Draw(sCmdList_, (uint32_t)rootParameterIndex::FOGDATA);

	// 描画コマンド
	sCmdList_->DrawInstanced(_countof(vertices_), 1, 0, 0); // 全ての頂点を使って描画
}

void DeferredRender::PreDrawScene(ID3D12GraphicsCommandList* cmdList)
{
	sCmdList_ = cmdList;

	// リソースバリアを作成
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_.Get(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);

	// リソースバリアを変更（シェーダリソース）描画可能
	cmdList->ResourceBarrier(1, &resourceBarrier);

	// レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH =
		handles_.rtvCpuHandle_;

	// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH =
		sDsvHandle_;

	// レンダーターゲットをセット
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	// ビューポートの設定
	CD3DX12_VIEWPORT viewPort = CD3DX12_VIEWPORT(0.0f, 0.0f, WinApp::window_width, WinApp::window_height);
	cmdList->RSSetViewports(1, &viewPort);

	// シザリング矩形の設定
	CD3DX12_RECT rect = CD3DX12_RECT(0, 0, WinApp::window_width, WinApp::window_height);
	cmdList->RSSetScissorRects(1, &rect);

	// 全画面クリア
	cmdList->ClearRenderTargetView(rtvH, clearColor_, 0, nullptr);

	// 深度バッファのクリア
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
}

void DeferredRender::PostDrawScene()
{
	// リソースバリアを作成
	CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(texBuff_.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	// リソースバリアを変更（描画可能シェーダーリソース）
	sCmdList_->ResourceBarrier(1, &resourceBarrier);
}

void DeferredRender::SetDevice(ID3D12Device* device)
{
	sDevice_ = device;
}

void DeferredRender::SetDsvHandle(D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle)
{
	sDsvHandle_ = dsvHandle;
}

void DeferredRender::CreateVertBuff()
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

void DeferredRender::CreateTex()
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
	result = sDevice_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&texresDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		&clearValue,
		IID_PPV_ARGS(&texBuff_));

	assert(SUCCEEDED(result));
}

void DeferredRender::CreateSRV()
{
	PostRenderBase::GetInstance()->CreateSRV(texBuff_.Get(), handles_.srvCpuHandle_, handles_.srvGpuHandle_);
}

void DeferredRender::CreateRTV()
{
	PostRenderBase::GetInstance()->CreateRTV(texBuff_.Get(), handles_.rtvCpuHandle_);
}

void DeferredRender::CreateDepthBuff()
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
	depthBuff_->SetName(L"DeferredDepth");
}

void DeferredRender::CreateDSV()
{
	PostRenderBase::GetInstance()->CreateDSV(depthBuff_.Get(), handles_.dsvCpuHandle_, DXGI_FORMAT_D32_FLOAT_S8X24_UINT);
}

void DeferredRender::CreateGraphicsPipelineState()
{
	result = S_FALSE;

	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	vsShader_ = std::make_unique<ShaderObj>();
	vsShader_->Create("Deferred/DeferredRenderVS.hlsl", "main", "vs_5_0", ShaderObj::ShaderType::VS);

	// ピクセルシェーダの読み込みとコンパイル
	psShader_ = std::make_unique<ShaderObj>();
	psShader_->Create("Deferred/DeferredRenderPS.hlsl", "main", "ps_5_0", ShaderObj::ShaderType::PS);

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			// x y 座標
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}, // (1行で書いたほうが見やすい)
		{
			// uv座標
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		}
	};

	// グラフィックスパイプライン設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	pipelineDesc.VS = *vsShader_->GetShader();
	pipelineDesc.PS = *psShader_->GetShader();

	// サンプルマスク
	pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;// 標準設定

	// ラスタライザステート
	pipelineDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

	// デプスステンシルステート
	pipelineDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;// 常に上書きルール

	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;// RBGA全てのチャンネルを描画

	// 共通の設定（アルファ値）
	blenddesc.BlendEnable = true;// ブレンドを有効にする

	// 半透明合成
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	// 共通の設定（アルファ値）
	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;// 加算
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;// ソースの値を100%使う
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;// デストの値を0%使う

	// ブレンドステートの設定
	pipelineDesc.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

	// 頂点レイアウトの設定
	pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	pipelineDesc.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	pipelineDesc.NumRenderTargets = 1;// 描画対象は一つ
	pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;// 0~255指定のRGBA
	pipelineDesc.SampleDesc.Count = 1;// 1ピクセルに付き1回サンプリング

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV0;
	descRangeSRV0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);// t0 レジスタ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV1;
	descRangeSRV1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);// t1 レジスタ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV2;
	descRangeSRV2.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);// t2 レジスタ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV3;
	descRangeSRV3.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);// t3 レジスタ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV4;
	descRangeSRV4.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);// t4 レジスタ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV5;
	descRangeSRV5.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);// t5 レジスタ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV6;
	descRangeSRV6.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 6);// t6 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootParams[9];
	rootParams[0].InitAsDescriptorTable(1, &descRangeSRV0, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[1].InitAsDescriptorTable(1, &descRangeSRV1, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[2].InitAsDescriptorTable(1, &descRangeSRV2, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[3].InitAsDescriptorTable(1, &descRangeSRV3, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[4].InitAsDescriptorTable(1, &descRangeSRV4, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[5].InitAsDescriptorTable(1, &descRangeSRV5, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[6].InitAsDescriptorTable(1, &descRangeSRV6, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[7].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParams[8].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0, D3D12_FILTER_MIN_MAG_MIP_POINT);
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootParams), rootParams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	Microsoft::WRL::ComPtr<ID3DBlob>rootSigBlob;

	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));

	// ルートシグネチャの生成
	result = sDevice_->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(result));

	pipelineDesc.pRootSignature = rootSignature_.Get();

	// グラフィックスパイプラインの生成
	result = sDevice_->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(result));
}
