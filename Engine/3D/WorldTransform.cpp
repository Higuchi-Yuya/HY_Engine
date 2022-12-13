#include "WorldTransform.h"
#include<cassert>
#include <d3dx12.h>

Microsoft::WRL::ComPtr<ID3D12Device> WorldTransform::device_ = nullptr;

void WorldTransform::StaticInitialize(ID3D12Device* device)
{
	assert(device);
	device_ = device;
}

void WorldTransform::Initialize()
{
	CreateConstBuffer();
	Map();
	UpdateMatrix();
}

void WorldTransform::CreateConstBuffer()
{
	assert(device_);

	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc =
		CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBufferDataWorldTransform) + 0xff) & ~0xff);

	HRESULT result;

	// 定数バッファの生成
	result = device_->CreateCommittedResource(
		&heapProps, // アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));

	assert(SUCCEEDED(result));
}

void WorldTransform::Map()
{
	//定数バッファのマッピング
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);//マッピング
	assert(SUCCEEDED(result));
}

void WorldTransform::UpdateMatrix()
{
	Matrix4 matScale, matRot, matTrans;
	Matrix4 matRotX, matRotY, matRotZ;
	matScale.identity();

	matTrans.identity();

	//各行列計算
	matScale.scale(scale_);
	matRot.identity();
	matRotZ.rotateZ(rotation_.z);
	matRotX.rotateX(rotation_.x);
	matRotY.rotateY(rotation_.y);
	matRot = matRotZ * matRotX * matRotY;
	matTrans.translate(position_);

	matWorld_.identity();
	matWorld_ *= matScale;
	matWorld_ *= matRot;
	matWorld_ *= matTrans;

	if (parent_ != nullptr) {
		matWorld_ *= parent_->matWorld_;
	}

	//定数バッファに転送
	constMap->color = color_;
	constMap->matWorld = matWorld_;
}
