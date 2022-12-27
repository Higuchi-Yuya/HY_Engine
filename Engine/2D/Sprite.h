#pragma once
#include "SpriteManager.h"
#include "Matrix4.h"
#include "Vector2.h"
#include "Vector4.h"

class Sprite
{
public:// サブクラス

	enum VertexNumber {
		LB,	//左下
		LT,	//左上
		RB,	//右下
		RT,	//右上
	};

public:// メンバ関数
	// スプライトマネージャーを借りる
	static void StaticInitialize(SpriteManager* spriteManager);

	// 初期化
	void Initialize(uint32_t textureNum = UINT32_MAX, Vector2 position = { 0.0f,0.0f }, Vector2 size = { 100.0f,100.0f },Vector4 color={1,1,1,1});

	// 更新処理
	void Updata();

	// 描画
	void Draw();

	// 座標設定
	void SetPosition(const Vector2& position) { position_ = position; }
	
	// 座標の取得
	const Vector2& GetPosition()const { return position_; }

	// 回転の設定
	void SetRotation(float rotation) { rotation_.z = rotation; }

	// 回転値の取得
	float GetRotation()const { return rotation_.z; }

	// 色の設定
	void SetColor(const Vector4& color) { color_ = color; }

	// 色の取得
	const Vector4& GetColor() const { return color_; }

	// サイズの設定
	void SetSize(const Vector2& size) { size_ = size; }

	// サイズの取得
	const Vector2& GetSize()const { return size_; }

	// アンカーポイントの設定
	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint;}

	// アンカーポイントの取得
	const Vector2& GetAnchorPoint()const { return anchorPoint_; }

	// 左右反転の設定
	void SetFlipX(const bool& isFlipX) { isFlipX_ = isFlipX; }

	// 左右反転の取得
	const bool& GetFlipX()const { return isFlipX_; }

	// 上下反転の設定
	void SetFlipY(const bool& isFlipY) { isFlipY_ = isFlipY; }

	// 上下反転の取得
	const bool& GetFlipY()const { return isFlipY_; }

	// 非表示の設定
	void SetInvisible(const bool& invisible) { isInvisible_ = invisible; }

	// 非表示のフラグの取得
	const bool& GetInvisible()const { return isInvisible_; }

	// テクスチャ切り出しの設定
	void SetTextureSize(const Vector2& textureSize) { textureSize_ = textureSize; }

	// 切り出しサイズの取得
	const Vector2& GetTextureSize()const { return textureSize_; }


public:// 静的メンバ変数

	static SpriteManager* spriteManager_;

private:// プライベートメンバ関数
	// テクスチャサイズをイメージに合わせる
	void AbjustTextureSize();

private:// メンバ変数

	SpriteManager::Vertex vertices[4] = {
	{{  0.0f,100.0f, 0.0f },{0.0f,1.0f}}, // 左下
	{{  0.0f,  0.0f, 0.0f },{0.0f,0.0f}}, // 左上
	{{100.0f,100.0f, 0.0f },{1.0f,1.0f}}, // 右下
	{{100.0f,  0.0f, 0.0f },{1.0f,0.0f}}, // 右上
	};

	Matrix4 matWorld;	//ワールド変換行列

	Vector3 rotation_ = { 0.0f,0.0f,0.0f };	    // 回転角

	Vector2 position_ = { 100.0f,100.0f };	// 座標
	
	Vector4 color_ = { 1,1,1,1 };	            // 色（RGBA）
	
	Vector2 size_ = { 100.0f,100.0f };          // スプライトサイズ
	
	Vector2 anchorPoint_ = { 0.5f,0.5f };       // アンカーポイント(座標変換の基準点)
	
	bool isFlipX_ = false;	                    // 左右反転フラグ
	
	bool isFlipY_ = false;	                    // 上下反転フラグ
	
	bool isInvisible_ = false;	                // 非表示フラグ

	// テクスチャ左上座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };

	// テクスチャ切り出しサイズ
	Vector2 textureSize_ = { 100.0f,100.0f };

	// テクスチャ番号
	uint32_t textureIndex_ = 0;

	SpriteManager::ConstBufferData* constMap = nullptr;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	// 頂点マップ
	SpriteManager::Vertex* vertMap = nullptr;

	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;

	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff = nullptr;

	// 結果確認
	HRESULT result;
};

