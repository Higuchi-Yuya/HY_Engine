#pragma once
#include"Vector3.h"
#include "Vector4.h"
#include<d3d12.h>
#include<wrl.h>

class Fog
{
public:// サブクラス
//定数バッファ用データ構造体
	struct ConstBufferDataFog {
		Vector4 fogColor; // fogの掛ける色
		bool isActiveFog; // フォグを掛けるかどうかのフラグ
		float nearFog;	  // fogの開始位置
		float farFog;	  // fogの終了位置
	};
public:// メンバ関数
	// 静的初期化
	static void StaticInitialize(ID3D12Device* device);

	static Fog* Create();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 行列を更新する
	/// </summary>
	void UpdateMatrix();

	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList);

	// バッファのゲッター
	ID3D12Resource* GetBuff() { return constBuff.Get(); }

private:// プライベート関数
	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// マッピングする
	/// </summary>
	void Map();

public:// 外側から変更可能な値
	bool isActiveFog = false;
	float nearFog = 0.0f;
	float farFog = 50.0f;
	Vector4 fogColor = { 1.0f,1.0f,1.0f,1.0f };

private:// メンバ変数
	// デバイス（借りてくる）
	static Microsoft::WRL::ComPtr<ID3D12Device> device_;

	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff;

	// マッピング済みアドレス
	ConstBufferDataFog* constMap = nullptr;

};

