#include "Sprite.h"
#include "WinApp.h"

SpriteManager* Sprite::sSpriteManager_ = nullptr;

void Sprite::Initialize(Texture* textureNum, Vector2 position, Vector2 size, Vector4 color)
{
	// 元の画像のサイズに適応
	if (textureNum != nullptr) {
		textureIndex_ = textureNum;
		AbjustTextureSize();
		//テクスチャサイズをスプライトのサイズに適用
		size_ = textureSize_;
	}

	// 引数で持ってきたものを代入
	position_.x = position.x;
	position_.y = position.y;
	size_ = size;
	color_ = color;

	// 頂点データ

	// 頂点データ全体のサイズ = 頂点データ一つ分のサイズ * 頂点データの要素数
	uint32_t sizeVB = static_cast<UINT>(sizeof(vertices_[0]) * _countof(vertices_));

	// 頂点バッファの設定
	D3D12_HEAP_PROPERTIES heapProp{}; // ヒープ設定
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; // GPUへの転送用
	// リソース設定
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; // 頂点データ全体のサイズ
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	// 頂点バッファの生成
	result = sSpriteManager_->sDxcommon_->GetDevice()->CreateCommittedResource(
		&heapProp, // ヒープ設定
		D3D12_HEAP_FLAG_NONE,
		&resDesc, // リソース設定
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));

	// GPU上のバッファに対応した仮想メモリ(メインメモリ上)を取得

	result = vertBuff_->Map(0, nullptr, (void**)&vertMap_);
	assert(SUCCEEDED(result));
	// 全頂点に対して
	for (int i = 0; i < _countof(vertices_); i++) {
		vertMap_[i] = vertices_[i]; // 座標をコピー
	}
	// 繋がりを解除
	vertBuff_->Unmap(0, nullptr);

	// 頂点バッファビューの作成
	
	// GPU仮想アドレス
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	// 頂点バッファのサイズ
	vbView_.SizeInBytes = sizeVB;
	// 頂点1つ分のデータサイズ
	vbView_.StrideInBytes = sizeof(vertices_[0]);

	//定数バッファの設定
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	//リソース設定
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(SpriteManager::ConstBufferData) + 0xff) & ~0xff;
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//定数バッファ生成
	result = sSpriteManager_->sDxcommon_->GetDevice()->CreateCommittedResource(
		&cbHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

	
	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));

	constMap_->color = Vector4(1, 0, 0, 1.0f);
	
	// 単位行列を代入
	constMap_->mat.identity();

	// 座標変換
	constMap_->mat.m[0][0] = 2.0f / WinApp::window_width;
	constMap_->mat.m[1][1] = -2.0f / WinApp::window_height;
	constMap_->mat.m[3][0] = -1.0f;
	constMap_->mat.m[3][1] = 1.0f;
}

Sprite::Sprite()
{

}

Sprite::Sprite(Texture textureNum, Vector2 position, Vector2 size, Vector4 color, Vector2 anchorPoint, bool isFlipX, bool isFlipY)
{
	textureIndex_ = &textureNum;
	position_ = position;
	size_ = size;
	color_ = color;
	anchorPoint_ = anchorPoint;
	isFlipX_ = isFlipX;
	isFlipY_ = isFlipY;
}

void Sprite::StaticInitialize(SpriteManager* spriteManager)
{
	sSpriteManager_ = spriteManager;
}

void Sprite::Updata(WorldTransform* parent)
{
	//頂点データをメンバ変数で計算
	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;

	// 左右反転
	if (isFlipX_) {
		left = -left;
		right = -right;
	}

	// 上下反転
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	// 頂点データの設定
	vertices_[LB].pos = { left,bottom,0.0f };
	vertices_[LT].pos = { left,top,0.0f };
	vertices_[RB].pos = { right,bottom,0.0f };
	vertices_[RT].pos = { right,top,0.0f };

	//テクスチャバッファ取得
	ID3D12Resource* textureBuffer = textureIndex_->buffer.Get();

	//指定番号のテクスチャが読み込み済みなら
	if (textureBuffer) {
		//テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

		float tex_left = textureLeftTop_.x / resDesc.Width;
		float tex_right = (textureSize_.x) / resDesc.Width;
		float tex_top = textureLeftTop_.y / resDesc.Height;
		float tex_bottom = (textureSize_.y) / resDesc.Height;

		//頂点のUVに反映
		vertices_[LB].uv = { tex_left,tex_bottom };
		vertices_[LT].uv = { tex_left,tex_top };
		vertices_[RB].uv = { tex_right,tex_bottom };
		vertices_[RT].uv = { tex_right,tex_top };
	}

	// 頂点データをGPUに転送
	std::copy(std::begin(vertices_), std::end(vertices_), vertMap_);

	// 行列の設定
	Matrix4 matScale;
	Matrix4 matRot;
	Matrix4 matTrans;

	Matrix4 matResult;

	matScale.identity();
	matRot.identity();
	matTrans.identity();

	matScale.scale(scale_);
	matRot.rotation(rotation_);
	matTrans.translate({ position_.x,position_.y,0.0f });

	Matrix4 matProjection;
	matProjection.identity();
	matProjection.m[0][0] = 2.0f / WinApp::window_width;
	matProjection.m[1][1] = -2.0f / WinApp::window_height;
	matProjection.m[3][0] = -1.0f;
	matProjection.m[3][1] = 1.0f;

	// 行列の掛け算
	matWorld_.identity();
	matWorld_ *= matScale;
	matWorld_ *= matRot;
	matWorld_ *= matTrans;
	// 親行列があった場合
	if (parent != nullptr)
	{
		//parent->matWorld_ *= matProjection;
		matWorld_ *= parent->matWorld_;
	}
	matResult = matWorld_ * matProjection;



	// 行列の転送
	constMap_->mat = matResult;

	// 色を転送
	constMap_->color = color_;
}

void Sprite::Draw()
{
	//非表示なら処理終了
	if (isInvisible_) {
		return;
	}

	sSpriteManager_->SetTextureCommands(textureIndex_);

	Updata();

	// 頂点バッファビューの設定コマンド
	sSpriteManager_->sDxcommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vbView_);

	// 定数バッファビュー(CBV)の設定コマンド
	sSpriteManager_->sDxcommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuff_->GetGPUVirtualAddress());

	// 描画コマンド
	sSpriteManager_->sDxcommon_->GetCommandList()->DrawInstanced(_countof(vertices_), 1, 0, 0); // 全ての頂点を使って描画
}

void Sprite::AbjustTextureSize()
{
	ID3D12Resource* textureBuffer = textureIndex_->buffer.Get();
	assert(textureBuffer);
	
	// テクスチャ情報取得
	D3D12_RESOURCE_DESC resDesc = textureBuffer->GetDesc();

	textureSize_.x = static_cast<float>(resDesc.Width);
	textureSize_.y = static_cast<float>(resDesc.Height);
}
