#pragma once
#include "SpriteManager.h"

class Sprite
{
	
public:// メンバ関数
	// 初期化
	void Initialize(SpriteManager *spriteManager);

	// 描画
	void Draw();

private:// メンバ変数

	DirectX::XMFLOAT3 vertices[3]={
	{ -0.5f, -0.5f, 0.0f }, // 左下
	{ -0.5f, +0.5f, 0.0f }, // 左上
	{ +0.5f, -0.5f, 0.0f }, // 右下
	};;

	SpriteManager* spriteManager_ = nullptr;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	// 結果確認
	HRESULT result;
};

