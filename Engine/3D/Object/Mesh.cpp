#include "Mesh.h"
#include <DirectXMath.h>
#include "TextureManager.h"

using namespace DirectX;

ID3D12Device* Mesh::sDevice_ = nullptr;

void Mesh::SetDevice(ID3D12Device* device)
{
	Mesh::sDevice_ = device;
}

void Mesh::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// nullptrチェック
	assert(sDevice_);
	assert(cmdList);

	// 頂点バッファの設定
	cmdList->IASetVertexBuffers(0, 1, &vbView_);
	// インデックスバッファの設定
	cmdList->IASetIndexBuffer(&ibView_);

	// デスクリプタヒープの配列
	ID3D12DescriptorHeap* ppHeaps[] = { TextureManager::sSrvHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	// マテリアル用定数バッファビューをセット
	cmdList->SetGraphicsRootConstantBufferView(3, material_->GetConstantBuffer()->GetGPUVirtualAddress());

	// シェーダリソースビューをセット
	cmdList->SetGraphicsRootDescriptorTable(0, material_->textureIndex.GetGpuHandle());

	// 描画コマンド
	cmdList->DrawIndexedInstanced((UINT)indices_.size(), 1, 0, 0, 0);
}

void Mesh::AddSmoothData(unsigned short indexPosition, unsigned short indexVertex)
{
	smoothData_[indexPosition].emplace_back(indexVertex);
}

void Mesh::CalculateSmoothedVertexNormals()
{
	auto itr = smoothData_.begin();
	for (; itr != smoothData_.end(); ++itr) {
		// 各面用の共通頂点コレクション
		std::vector<unsigned short>& v = itr->second;

		// 全頂点の法線を平均する
		XMVECTOR normal = {};
		for (unsigned short index : v) {
			normal += XMVectorSet(vertices_[index].normal.x, vertices_[index].normal.y, vertices_[index].normal.z, 0);
		}
		normal = XMVector3Normalize(normal / (float)v.size());

		// 共通法線を使用するすべての頂点データに書き込む
		for (unsigned short index : v) {
			vertices_[index].normal = { normal.m128_f32[0],normal.m128_f32[1],normal.m128_f32[2] };
		}
	}
}

void Mesh::SetName(const std::string& name)
{
	name_ = name;
}

void Mesh::AddVertex(const VertexPosNormalUv& vertex)
{
	vertices_.emplace_back(vertex);
}

void Mesh::AddIndex(unsigned short index)
{
	indices_.emplace_back(index);
}

void Mesh::SetMaterial(Material* material)
{
	material_ = material;
}

void Mesh::CreateBuffers()
{
	HRESULT result = S_FALSE;

	uint32_t sizeVB = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices_.size());//vertices_
	uint32_t sizeIB = static_cast<UINT>(sizeof(unsigned short) * indices_.size());

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);
	CD3DX12_RESOURCE_DESC resourceDesc2 = CD3DX12_RESOURCE_DESC::Buffer(sizeIB);
	// 頂点バッファ生成
	result = sDevice_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));

	// 頂点バッファへのデータ転送
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}

	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);

	// リソース設定
	resourceDesc.Width = sizeIB;

	// インデックスバッファ生成
	result = sDevice_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc2, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&indexBuff_));

	// インデックスバッファへのデータ転送
	unsigned short* indexMap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		std::copy(indices_.begin(), indices_.end(), indexMap);
		indexBuff_->Unmap(0, nullptr);
	}

	// インデックスバッファビューの作成
	ibView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB;

	// リソース設定
	CD3DX12_RESOURCE_DESC BoneResourceDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(Bone) + 0xff) & ~0xff);

	// 定数バッファの生成
	result = sDevice_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE, &BoneResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&BoneBuff_));

	// 定数バッファとのデータリンク
	result = BoneBuff_->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));

	//// リソースデスクをマテリアル用バッファにリサイズ
	//resourceDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataB1) + 0xff) & ~0xff);
}
