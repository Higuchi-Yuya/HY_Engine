#pragma once

#include"Vector3.h"
#include "Vector4.h"
#include"Matrix4.h"
#include<d3d12.h>
#include<wrl.h>


class WorldTransform
{
public:// サブクラス
	//定数バッファ用データ構造体
	struct ConstBufferDataWorldTransform {
		Vector4 color;
		Matrix4 matWorld;// 行列
		unsigned int activeBloom;
		float pad;
	};
public:// メンバ関数
	// 静的初期化
	static void StaticInitialize(ID3D12Device* device);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 行列を更新する
	/// </summary>
	void UpdateMatrix();

	// バッファのゲッター
	ID3D12Resource *GetBuff() { return constBuff_.Get(); }

private:// プライベート関数
	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// マッピングする
	/// </summary>
	void Map();

public:// パブリック変数
	// ローカルスケール
	Vector3 scale = { 1.0f, 1.0f, 1.0f };

	// X,Y,Z軸回りのローカル回転角
	Vector3 rotation = { 0, 0, 0 };

	// ローカル座標
	Vector3 translation = { 0, 0, 0 };

	// 色
	Vector4 color = { 1,1,1,1 };

	// ローカル → ワールド変換行列
	Matrix4 matWorld_;

	// 親となるワールド変換へのポインタ
	const WorldTransform* parent_ = nullptr;

	// 一番小さい頂点と大きい頂点
	Vector3 minVertex_;
	Vector3 maxVertex_;

	// ブルームなのかどうか
	bool IsBloom_ = false;

private:// メンバ変数
	// デバイス（借りてくる）
	static ID3D12Device* device_;

	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;

	// マッピング済みアドレス
	ConstBufferDataWorldTransform* constMap_ = nullptr;
};

