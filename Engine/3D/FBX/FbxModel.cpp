#include "DirectXCommon.h"
#include "FbxModel.h"
#include <algorithm>
#include <cassert>
#include <d3dcompiler.h>
#include <fstream>
#include <sstream>

#include<atldef.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace Microsoft::WRL;
using namespace DirectX;

/// <summary>
/// 静的メンバ変数の実体
/// </summary>
const std::string FbxModel::kBaseDirectory_ = "Resources/";
const std::string FbxModel::kDefaultModelName_ = "cube";
UINT FbxModel::sDescriptorHandleIncrementSize_ = 0;
ID3D12GraphicsCommandList* FbxModel::sCommandList_ = nullptr;
ComPtr<ID3D12RootSignature> FbxModel::sRootSignature_;
ComPtr<ID3D12PipelineState> FbxModel::sPipelineState_;
std::unique_ptr<LightGroup> FbxModel::lightGroup_;
ID3D12Device* FbxModel::device_;

Microsoft::WRL::ComPtr<ID3D12Resource> FbxModel::constBuffSkin_;
Microsoft::WRL::ComPtr<ID3D12Resource> FbxModel::constBuffNothing_;

void FbxModel::StaticInitialize() {

	// パイプライン初期化
	InitializeGraphicsPipeline();

	// ライト生成
	lightGroup_.reset(LightGroup::Create());
}

void FbxModel::StaticFainalize()
{

	sRootSignature_.Reset();
	sPipelineState_.Reset();

	constBuffSkin_.Reset();
	constBuffNothing_.Reset();
}

void FbxModel::InitializeGraphicsPipeline() {
	HRESULT result = S_FALSE;
	ComPtr<ID3DBlob> vsBlob;    // 頂点シェーダオブジェクト
	ComPtr<ID3DBlob> gsBlob;    //ジオメトリシェーダーオブジェクト
	ComPtr<ID3DBlob> psBlob;    // ピクセルシェーダオブジェクト
	ComPtr<ID3DBlob> errorBlob; // エラーオブジェクト


	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/FBXVS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0, &vsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n(
			(char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/FBXGS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "gs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0, &gsBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n(
			(char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}


	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/shaders/FBXPS.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"main", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0, &psBlob, &errorBlob);
	if (FAILED(result)) {
		// errorBlobからエラー内容をstring型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());

		std::copy_n(
			(char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
	  {// xy座標(1行で書いたほうが見やすい)
	   "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	  {// 法線ベクトル(1行で書いたほうが見やすい)
	   "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	  {// uv座標(1行で書いたほうが見やすい)
	   "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT,
	   D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},

	  {//影響を受けるボーン番号
		"BONEINDICES",0,DXGI_FORMAT_R32G32B32A32_UINT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	  },
	  {//ボーンのスキンウェイト(4つ)
		"BONEWEIGHTS",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
	  },

	};

	// グラフィックスパイプラインの流れを設定
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob.Get());
	gpipeline.GS = CD3DX12_SHADER_BYTECODE(gsBlob.Get());
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob.Get());

	// サンプルマスク
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	// ラスタライザステート
	//gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	// ラスタライザステート
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpipeline.RasterizerState.DepthClipEnable = true;


	// gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//  デプスステンシルステート
	//gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	//  デプスステンシルステート
	gpipeline.DepthStencilState.DepthEnable = true;
	gpipeline.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;


	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // RBGA全てのチャンネルを描画
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドステートの設定
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	// 深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 頂点レイアウトの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	// 図形の形状設定（三角形）
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	gpipeline.NumRenderTargets = 1;                       // 描画対象は1つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT; // 0～255指定のRGBA
	gpipeline.SampleDesc.Count = 1; // 1ピクセルにつき1回サンプリング

	// デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 レジスタ

	// ルートパラメータ
	CD3DX12_ROOT_PARAMETER rootparams[7];
	rootparams[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[3].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[4].InitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[5].InitAsConstantBufferView(4, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootparams[6].InitAsConstantBufferView(5, 0, D3D12_SHADER_VISIBILITY_ALL);

	// スタティックサンプル
	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	// ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(
		_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	// バージョン自動判定のシリアライズ
	result = D3DX12SerializeVersionedRootSignature(
		&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &rootSigBlob, &errorBlob);
	// ルートシグネチャの生成
	result = device_->CreateRootSignature(
		0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&sRootSignature_));
	assert(SUCCEEDED(result));

	gpipeline.pRootSignature = sRootSignature_.Get();

	// グラフィックスパイプラインの生成
	result = device_->CreateGraphicsPipelineState(
		&gpipeline, IID_PPV_ARGS(&sPipelineState_));
	assert(SUCCEEDED(result));


	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataSkin) + 0xff) & ~0xff);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDescNothing = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataInitialMatrix) + 0xff) & ~0xff);

	// 定数バッファの生成
	result = device_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuffSkin_));

	// 定数バッファの生成
	result = device_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDescNothing, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuffNothing_));

	Matrix4 mathMat;
	

	//定数バッファへデータ転送
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin_->Map(0, nullptr, (void**)&constMapSkin);
	for (int i = 0; i < MAX_BONES; i++) {
		constMapSkin->bones[i] = mathMat.identity();
	}
	constBuffSkin_->Unmap(0, nullptr);

}

FbxModel* FbxModel::Create() {
	// メモリ確保
	FbxModel* instance = new FbxModel;
	instance->Initialize();

	return instance;
}

FbxModel* FbxModel::CreateFromFbx(const std::string& modelname, bool smoothing) {
	// メモリ確保
	FbxModel* instance = new FbxModel;
	instance->Initialize();

	return instance;
}

void FbxModel::PreDraw(ID3D12GraphicsCommandList* commandList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(FbxModel::sCommandList_ == nullptr);

	// コマンドリストをセット
	sCommandList_ = commandList;

	// パイプラインステートの設定
	commandList->SetPipelineState(sPipelineState_.Get());
	// ルートシグネチャの設定
	commandList->SetGraphicsRootSignature(sRootSignature_.Get());
	// プリミティブ形状を設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void FbxModel::PostDraw() {
	// コマンドリストを解除
	sCommandList_ = nullptr;
}

FbxModel::~FbxModel() {
	for (auto m : meshes_) {
		delete m;
	}
	meshes_.clear();

	for (auto m : materials_) {
		delete m.second;
	}
	materials_.clear();

	delete defaultMaterial_;
}

void FbxModel::Initialize() {
	// モデル読み込み
	//LoadModelFromFile(modelname);

	// メッシュのマテリアルチェック
	for (auto& m : meshes_) {
		// マテリアルの割り当てがない
		if (m->GetMaterial() == nullptr) {
			if (defaultMaterial_ == nullptr) {
				// デフォルトマテリアルを生成
				defaultMaterial_ = Material::Create();
				defaultMaterial_->name = "no material";
				materials_.emplace(defaultMaterial_->name, defaultMaterial_);
			}
			// デフォルトマテリアルをセット
			m->SetMaterial(defaultMaterial_);
		}
	}

	// メッシュのバッファ生成
	for (auto& m : meshes_) {

		Vector3 ambient_ = { 1.0f, 1.0f, 1.0f };
		Vector3 diffuse_ = { 0.0f, 0.0f, 0.0f };
		Vector3 specular_ = { 0.0f, 0.0f, 0.0f };
		float alpha_ = 1.0f;



		m->CreateBuffers();
	}

	// マテリアルの数値を定数バッファに反映
	for (auto& m : materials_) {

		m.second->Update();
	}

	for (auto& m : meshes_) {
		for (auto& b : m->) {

			m->bones[b.name] = &b;

		}

	}


	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataWorldTransform) + 0xff) & ~0xff);

	// 定数バッファの生成
	HRESULT result = device_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

	//定数バッファのマッピング
	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));

	constMap_->Destruction = 0.0f;
	constMap_->ScaleFactor = 1.0f;
	constMap_->PositionFactor = 0.0f;
	constMap_->RotationFactor = 0.0f;
}

void FbxModel::FbxUpdate(float frem)
{
	HRESULT result = S_FALSE;

	//std::unordered_map<std::string, Mesh::Bone*> bones = model->GetBones();

	////定数バッファへのデータ転送
	//ConstBufferDataSkin* constMapSkin = nullptr;
	//result = constBuffSkin->Map(0, nullptr, (void**)&constMapSkin);
	//for (int i = 0; i < bones.size(); i++) {
	//	//今の姿勢行列
	//	XMMATRIX matCurrentPose;
	//	//今の姿勢行列を取得
	//	FbxAMatrix fbxCurrentPose = bones[i].fbxCluster->GetLink()->EvaluateGlobalTransform(currentTime);
	//	//XMMATRIXに変換
	//	FbxLoader::ConvertMatrixFromFbx(&matCurrentPose, fbxCurrentPose);
	//	//合成してスキニング行列に
	//	constMapSkin->bones[i] = bones[i].invInitialPose * matCurrentPose;
	//}
	//constBuffSkin->Unmap(0, nullptr);

}


void FbxModel::Draw(
	const WorldTransform& worldTransform, const ViewProjection& viewProjection) {

	for (int i = 0; i < meshes_.size(); i++) {

		// ライトの描画
		lightGroup_->Draw(sCommandList_);

		// CBVをセット（ワールド行列）
		sCommandList_->SetGraphicsRootConstantBufferView(0, worldTransform.GetGPUVirtualAddress());

		// CBVをセット（ビュープロジェクション行列）
		sCommandList_->SetGraphicsRootConstantBufferView(1, viewProjection.constBuff_->GetGPUVirtualAddress());

		if (meshes_[i]->node) {

			HRESULT result = S_FALSE;

			//定数バッファへデータ転送
			ConstBufferDataInitialMatrix* constMapSkin = nullptr;
			result = constBuffNothing_->Map(0, nullptr, (void**)&constMapSkin);
			constMapSkin->InitialMatrix = meshes_[i]->node_->globalTransform;

			constBuffNothing_->Unmap(0, nullptr);


			sCommandList_->SetGraphicsRootConstantBufferView(5, constBuffNothing_->GetGPUVirtualAddress());

		}

		// CBVをセット（ボーン行列）
		sCommandList_->SetGraphicsRootConstantBufferView(4, constBuffSkin_->GetGPUVirtualAddress());

		//CBVをセット（ポリゴン爆散）
		sCommandList_->SetGraphicsRootConstantBufferView(6, constBuff_->GetGPUVirtualAddress());

		// 全メッシュを描画
		meshes_[i]->Draw(sCommandList_, 2, 3, modelTextureHandle);
	}
}


void FbxModel::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textureHadle) {

	// 全メッシュを描画
	for (auto& mesh : meshes_) {

		// ライトの描画
		lightGroup_->Draw(sCommandList_);

		// CBVをセット（ワールド行列）
		sCommandList_->SetGraphicsRootConstantBufferView(0, worldTransform.constBuff_->GetGPUVirtualAddress());

		// CBVをセット（ビュープロジェクション行列）
		sCommandList_->SetGraphicsRootConstantBufferView(1, viewProjection.constBuff_->GetGPUVirtualAddress());

		if (mesh->node) {

			HRESULT result = S_FALSE;

			//定数バッファへデータ転送
			ConstBufferDataInitialMatrix* constMapSkin = nullptr;
			result = constBuffNothing_->Map(0, nullptr, (void**)&constMapSkin);
			constMapSkin->InitialMatrix = mesh->node->globalTransform;

			constBuffNothing_->Unmap(0, nullptr);


			sCommandList_->SetGraphicsRootConstantBufferView(5, constBuffNothing_->GetGPUVirtualAddress());

		}

		// CBVをセット（ボーン行列）
		sCommandList_->SetGraphicsRootConstantBufferView(4, constBuffSkin_->GetGPUVirtualAddress());

		//CBVをセット（ポリゴン爆散）
		sCommandList_->SetGraphicsRootConstantBufferView(6, constBuff_->GetGPUVirtualAddress());


		mesh->Draw(sCommandList_, 2, 3, textureHadle);
	}
}


void FbxModel::ModelAnimation(float frame, aiAnimation* Animation, int BoneNum) {

	HRESULT result = S_FALSE;

	Matrix4 mxIdentity = MyMath::MakeIdentity();
	Node* pNode = &nodes[0];

	naosi.Initialize();

	FLOAT TicksPerSecond = (FLOAT)(Animation->mTicksPerSecond != 0 ? Animation->mTicksPerSecond : 25.0f);

	FLOAT TimeInTicks = frame * TicksPerSecond;
	FLOAT AnimationTime = fmod(TimeInTicks, (FLOAT)Animation->mDuration);

	//定数バッファへデータ転送
	ConstBufferDataSkin* constMapSkin = nullptr;
	result = constBuffSkin_->Map(0, nullptr, (void**)&constMapSkin);

	for (Mesh* mesh : meshes_)
	{
		ReadNodeHeirarchy(mesh, Animation, AnimationTime, pNode, mxIdentity);

		UINT nNumBones = (UINT)mesh->bones.size();

		for (UINT i = 0; i < nNumBones; i++)
		{
			mesh->vecBones[i].matrix = mesh->bones[mesh->vecBones[i].name]->matrix;

			constMapSkin->bones[i] = mesh->vecBones[i].matrix;
		}
	}

	for (Mesh* mesh : meshes_)
	{
		/*naosi.translation_ = MyMath::GetWorldTransform();
		naosi.translation_ = naosi.translation_;
		naosi.TransferMatrix();*/

		matrixL_ = mesh->bones[mesh->vecBones[16].name]->matrix;
		matrixR_ = mesh->bones[mesh->vecBones[40].name]->matrix;

	}
	constBuffSkin_->Unmap(0, nullptr);
}


void FbxModel::ReadNodeHeirarchy(Mesh* mesh, aiAnimation* pAnimation, FLOAT AnimationTime, Node* pNode, Matrix4& mxParentTransform) {

	Matrix4 mxNodeTransformation = MyMath::MakeIdentity();
	mxNodeTransformation = pNode->transform;

	Matrix4 mxThisTrans = mxNodeTransformation;

	std::string strNodeName(pNode->name);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, strNodeName);

	if (pNodeAnim)
	{
		//スケーリング
		Vector3 vScaling = {};
		CalcInterpolatedScaling(vScaling, AnimationTime, pNodeAnim);
		Matrix4 mxScaling;
		mxScaling = MyMath::Scale(vScaling);

		//回転角
		Vector4 vRotationQ = {};
		CalcInterpolatedRotation(vRotationQ, AnimationTime, pNodeAnim);
		Matrix4 mxRotationM = Quaternion(vRotationQ).Rotate();

		//移動
		Vector3 vTranslation = {};
		CalcInterpolatedPosition(vTranslation, AnimationTime, pNodeAnim);
		Matrix4 mxTranslationM;
		mxTranslationM = MyMath::Translation(vTranslation);

		Matrix4 affin = MyMath::Initialize();
		/*affin *= mxScaling;
		affin *= mxRotationM;
		affin *= mxTranslationM;*/

		affin = mxScaling.MatMul(mxRotationM).MatMul(mxTranslationM);

		mxNodeTransformation = affin;
	}

	Matrix4 mxGlobalTransformation = mxNodeTransformation.MatMul(mxParentTransform);

	Matrix4 offsetMatirx;
	Matrix4 matirx;

	if (mesh->bones.find(strNodeName) != mesh->bones.end())
	{
		offsetMatirx = mesh->bones[strNodeName]->offsetMatirx;

		matirx = offsetMatirx * mxGlobalTransformation * globalInverseTransform;

		mesh->bones[strNodeName]->matrix = matirx;

	}

	for (UINT i = 0; i < pNode->childrens.size(); i++)
	{
		ReadNodeHeirarchy(mesh
			, pAnimation
			, AnimationTime
			, pNode->childrens[i]
			, mxGlobalTransformation);
	}



}


aiNodeAnim* FbxModel::FindNodeAnim(const aiAnimation* pAnimation, const std::string& strNodeName)
{
	for (UINT i = 0; i < pAnimation->mNumChannels; i++)
	{
		if (std::string(pAnimation->mChannels[i]->mNodeName.data) == strNodeName)
		{
			return pAnimation->mChannels[i];
		}
	}

	return nullptr;
}

void FbxModel::CalcInterpolatedScaling(Vector3& mxOut, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		mxOut = MyMath::AssimpVector3(pNodeAnim->mScalingKeys[0].mValue);
		return;
	}

	UINT ScalingIndex = 0;
	if (!FindScaling(AnimationTime, pNodeAnim, ScalingIndex))
	{
		mxOut = Vector3(1.0f, 1.0f, 1.0f);
		return;
	}

	UINT NextScalingIndex = (ScalingIndex + 1);
	ATLASSERT(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	ATLASSERT(Factor >= 0.0f && Factor <= 1.0f);

	mxOut = lerp(MyMath::AssimpVector3(pNodeAnim->mScalingKeys[ScalingIndex].mValue), MyMath::AssimpVector3(pNodeAnim->mScalingKeys[NextScalingIndex].mValue), Factor);

}

void FbxModel::CalcInterpolatedRotation(Vector4& mxOut, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumRotationKeys == 1)
	{
		mxOut = MyMath::AssimpQuaternionVec4(pNodeAnim->mRotationKeys[0].mValue);
		return;
	}

	UINT RotationIndex = 0;
	if (!FindRotation(AnimationTime, pNodeAnim, RotationIndex))
	{
		mxOut = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		return;
	}

	UINT NextRotationIndex = (RotationIndex + 1);
	ATLASSERT(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime
		- pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	ATLASSERT(Factor >= 0.0f && Factor <= 1.0f);

	mxOut = MyMath::QuaternionSlerp(
		pNodeAnim->mRotationKeys[RotationIndex].mValue
		, pNodeAnim->mRotationKeys[NextRotationIndex].mValue
		, Factor);

}

void FbxModel::CalcInterpolatedPosition(Vector3& mxOut, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		mxOut = MyMath::AssimpVector3(pNodeAnim->mPositionKeys[0].mValue);
		return;
	}

	UINT PositionIndex = 0;
	if (!FindPosition(AnimationTime, pNodeAnim, PositionIndex))
	{
		mxOut = Vector3(0.0f, 0.0f, 0.0f);
		return;
	}

	UINT NextPositionIndex = (PositionIndex + 1);

	ATLASSERT(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	ATLASSERT(Factor >= 0.0f && Factor <= 1.0f);

	mxOut = lerp(MyMath::AssimpVector3(pNodeAnim->mPositionKeys[PositionIndex].mValue), MyMath::AssimpVector3(pNodeAnim->mPositionKeys[NextPositionIndex].mValue), Factor);
}

bool FbxModel::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, UINT& nPosIndex)
{
	nPosIndex = 0;
	if (!(pNodeAnim->mNumPositionKeys > 0))
	{
		return FALSE;
	}

	for (UINT i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
	{
		if ((AnimationTime >= (float)pNodeAnim->mPositionKeys[i].mTime)
			&& (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime))
		{
			nPosIndex = i;
			return TRUE;
		}
	}

	return FALSE;
}

Matrix4 FbxModel::GetLeftBonePos()
{
	return matrixL_;
}

Matrix4 FbxModel::GetRightBonePos()
{
	return matrixR_;
}



bool FbxModel::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, UINT& nRotationIndex)
{
	nRotationIndex = 0;
	if (!(pNodeAnim->mNumRotationKeys > 0))
	{
		return FALSE;
	}

	for (UINT i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{

		if ((AnimationTime >= (float)pNodeAnim->mRotationKeys[i].mTime)
			&& (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime))
		{
			nRotationIndex = i;
			return TRUE;
		}
	}

	return FALSE;
}

bool FbxModel::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, UINT& nScalingIndex)
{
	nScalingIndex = 0;
	if (!(pNodeAnim->mNumScalingKeys > 0))
	{
		return FALSE;
	}

	for (UINT i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
	{
		if ((AnimationTime >= (float)pNodeAnim->mScalingKeys[i].mTime)
			&& (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime))
		{
			nScalingIndex = i;
			return TRUE;
		}
	}

	return FALSE;
}