#include "BillboardTex.h"
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <MathUtil.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
ID3D12Device* BillboardTex::sDevice_ = nullptr;
ID3D12GraphicsCommandList* BillboardTex::sCmdList_ = nullptr;
ComPtr<ID3D12RootSignature> BillboardTex::sRootsignature_;
ComPtr<ID3D12PipelineState> BillboardTex::sPipelinestate_;
ComPtr<ID3D12Resource> BillboardTex::sVertBuff_;
ComPtr<ID3D12Resource> BillboardTex::sIndexBuff_;

D3D12_VERTEX_BUFFER_VIEW BillboardTex::vbView{};
D3D12_INDEX_BUFFER_VIEW BillboardTex::ibView{};
BillboardTex::VertexPosNormalUv BillboardTex::vertices[vertexCount];
//unsigned short BillboardTex::indices[planeCount * 3];
unsigned short BillboardTex::indices[indexCount];
ViewProjection* BillboardTex::sViewProjection_ = nullptr;

void BillboardTex::StaticInitialize(ID3D12Device* device)
{
	// nullptrチェック
	assert(device);

	sDevice_ = device;

	// デスクリプタヒープの初期化
	InitializeDescriptorHeap();

	// パイプライン初期化
	InitializeGraphicsPipeline();

	// モデル生成
	CreateModel();

}

void BillboardTex::StaticFinalize()
{
	sRootsignature_ = nullptr;
	sPipelinestate_ = nullptr;
	sVertBuff_ = nullptr; 
	sIndexBuff_ = nullptr;
}

void BillboardTex::SetViewProjection(ViewProjection* viewProjection)
{
	sViewProjection_ = viewProjection;
}

void BillboardTex::PreDraw(ID3D12GraphicsCommandList* cmdList)
{

	// コマンドリストをセット
	sCmdList_ = cmdList;

	// パイプラインステートの設定
	sCmdList_->SetPipelineState(sPipelinestate_.Get());
	// ルートシグネチャの設定
	sCmdList_->SetGraphicsRootSignature(sRootsignature_.Get());
	// プリミティブ形状を設定
	sCmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void BillboardTex::PostDraw()
{
	// コマンドリストを解除
	BillboardTex::sCmdList_ = nullptr;
}

BillboardTex* BillboardTex::Create()
{
	// 3Dオブジェクトのインスタンスを生成
	BillboardTex* billboardTex = new BillboardTex();
	if (billboardTex == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!billboardTex->Initialize()) {
		delete billboardTex;
		assert(0);
		return nullptr;
	}

	return billboardTex;
}

void BillboardTex::InitializeDescriptorHeap()
{

}

void BillboardTex::InitializeGraphicsPipeline()
{
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob; // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> psBlob;	// ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/Billboard/BasicVertexShader.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/Billboard/BasicPixelShader.hlsl",	// シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0",	// エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{ // xy座標(1行で書いたほうが見やすい)
			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // 法線ベクトル(1行で書いたほうが見やすい)
			"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
		{ // uv座標(1行で書いたほうが見やすい)
			"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
		},
	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

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
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;
	gpipeline.BlendState.AlphaToCoverageEnable = true;
	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;	// 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[3];
	//テクスチャレジスタ0番 ---テクスチャシェーダーリソースビュー用
	rootparams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;	//種類
	rootparams[0].DescriptorTable.pDescriptorRanges = &descRangeSRV;			//デスクリプタレンジ
	rootparams[0].DescriptorTable.NumDescriptorRanges = 1;						//デスクリプタレンジ数
	rootparams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;				//すべてのシェーダから見える

	rootparams[1].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプラー
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ルートシグネチャの生成
	result = sDevice_->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(), IID_PPV_ARGS(&sRootsignature_));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = sRootsignature_.Get();

	// グラフィックスパイプラインの生成
	result = sDevice_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&sPipelinestate_));
	assert(SUCCEEDED(result));

}

void BillboardTex::CreateModel()
{
	HRESULT result = S_FALSE;

	std::vector<VertexPosNormalUv> realVertices;

	VertexPosNormalUv verticesSquare[] = {
		{{-5.0f,-5.0f,0.0f},{0,0,1},{0,1}},// 左下
		{{-5.0f,+5.0f,0.0f},{0,0,1},{0,0}},// 左上
		{{+5.0f,-5.0f,0.0f},{0,0,1},{1,1}},// 右下
		{{+5.0f,+5.0f,0.0f},{0,0,1},{1,0}},// 右上
	};

	std::copy(std::begin(verticesSquare), std::end(verticesSquare), vertices);

	// 四角形のインデックスデータ
	unsigned short indicesSquare[] = {
		0,1,2,
		2,1,3,
	};

	// メンバ変数にコピー
	std::copy(std::begin(indicesSquare), std::end(indicesSquare), indices);

	UINT sizeVB = static_cast<UINT>(sizeof(vertices));

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

	// 頂点バッファ生成
	result = sDevice_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&sVertBuff_));
	assert(SUCCEEDED(result));

	// 頂点バッファへのデータ転送
	VertexPosNormalUv* vertMap = nullptr;
	result = sVertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		memcpy(vertMap, vertices, sizeof(vertices));
		sVertBuff_->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView.BufferLocation = sVertBuff_->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(vertices);
	vbView.StrideInBytes = sizeof(vertices[0]);

	UINT sizeIB = static_cast<UINT>(sizeof(indices));
	// リソース設定
	resourceDesc.Width = sizeIB;

	// インデックスバッファ生成
	result = sDevice_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&sIndexBuff_));

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = sIndexBuff_->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {

		// 全インデックスに対して
		for (int i = 0; i < _countof(indices); i++)
		{
			indexMap[i] = indices[i];	// インデックスをコピー
		}

		sIndexBuff_->Unmap(0, nullptr);
	}

	// インデックスバッファビューの作成
	ibView.BufferLocation = sIndexBuff_->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeof(indices);
}

void BillboardTex::UpdateViewMatrix()
{
	//視点座標
	Vector3 eyePosition = sViewProjection_->eye;
	//注視点座標X
	Vector3 targetPosition = sViewProjection_->target;
	//(仮の)上方向
	Vector3 upVector = sViewProjection_->up;

	//カメラZ軸(視点方向)
	Vector3 cameraAxisZ = targetPosition - eyePosition;

	//ベクトルを正規化
	cameraAxisZ.normalize();

	//カメラのX軸(右方向)
	Vector3 cameraAxisX;
	//X軸は上方向→Z軸の外積で求まる
	cameraAxisX = upVector.cross(cameraAxisZ);
	//ベクトルを正規化
	cameraAxisX.normalize();

	//カメラのY軸(上方向)
	Vector3 cameraAxisY;
	//Y軸は上方向→Z軸の外積で求まる
	cameraAxisY = cameraAxisZ.cross(cameraAxisX);
	//ベクトルを正規化
	cameraAxisY.normalize();

	matBillboard_.get()->identity();

	switch (bType_)
	{
	case BillboardTex::AllAxisBillboard:
	{
	#pragma region 全方向ビルボード行列の計算
		//ビルボード行列
		matBillboard_->m[0][0] = cameraAxisX.x;
		matBillboard_->m[0][1] = cameraAxisX.y;
		matBillboard_->m[0][2] = cameraAxisX.z;

		matBillboard_->m[1][0] = cameraAxisY.x;
		matBillboard_->m[1][1] = cameraAxisY.y;
		matBillboard_->m[1][2] = cameraAxisY.z;

		matBillboard_->m[2][0] = cameraAxisZ.x;
		matBillboard_->m[2][1] = cameraAxisZ.y;
		matBillboard_->m[2][2] = cameraAxisZ.z;
	#pragma endregion
		break;
	}
	case BillboardTex::YAxisBillboard:
	{
	#pragma region Y軸周りビルボード行列の計算
		// カメラX軸、Y軸、Z軸
		Vector3 ybillCameraAxisX, ybillCameraAxisY, ybillCameraAxisZ;

		// X軸は共通
		ybillCameraAxisX = cameraAxisX;
		// Y軸はワールド座標系のY軸
		ybillCameraAxisY = upVector.normalize();
		// Z軸はX軸→Y軸の外積で求まる
		ybillCameraAxisZ = Vector3::Cross(cameraAxisX, cameraAxisY);

		// Y軸周りビルボード行列
		matBillboard_->m[0][0] = ybillCameraAxisX.x;
		matBillboard_->m[0][1] = ybillCameraAxisX.y;
		matBillboard_->m[0][2] = ybillCameraAxisX.z;

		matBillboard_->m[1][0] = ybillCameraAxisY.x;
		matBillboard_->m[1][1] = ybillCameraAxisY.y;
		matBillboard_->m[1][2] = ybillCameraAxisY.z;

		matBillboard_->m[2][0] = ybillCameraAxisZ.x;
		matBillboard_->m[2][1] = ybillCameraAxisZ.y;
		matBillboard_->m[2][2] = ybillCameraAxisZ.z;

	#pragma endregion
		break;
	}
	default:
		break;
	}
}

bool BillboardTex::Initialize()
{
	// nullptrチェック
	assert(sDevice_);

	worldTransform_.Initialize();
	matBillboard_ = std::make_unique<Matrix4>();
	matBillboard_->identity();
	worldTransform_.SetBillboardMat(matBillboard_.get());

	return true;
}

void BillboardTex::Update()
{
	UpdateViewMatrix();
	worldTransform_.UpdateMatrix();
}

void BillboardTex::Draw()
{
	// nullptrチェック
	assert(sDevice_);
	assert(BillboardTex::sCmdList_);

	// 頂点バッファの設定
	sCmdList_->IASetVertexBuffers(0, 1, &vbView);
	// インデックスバッファの設定
	sCmdList_->IASetIndexBuffer(&ibView);

	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { TextureManager::sSrvHeap.Get() };
	sCmdList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// シェーダリソースビューをセット
	sCmdList_->SetGraphicsRootDescriptorTable(0, textureHandle_->GetGpuHandle());

	// 定数バッファビューをセット
	sCmdList_->SetGraphicsRootConstantBufferView(1, worldTransform_.GetBuff()->GetGPUVirtualAddress());
	sCmdList_->SetGraphicsRootConstantBufferView(2, sViewProjection_->GetBuff()->GetGPUVirtualAddress());

	// 描画コマンド
	sCmdList_->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);
}

void BillboardTex::SetTexture(Texture* tex)
{
	textureHandle_ = tex;
}
