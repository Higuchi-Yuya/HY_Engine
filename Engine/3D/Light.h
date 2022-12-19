#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <d3d12.h>
#include<wrl.h>
class Light
{
private:
	// namespaceの省略
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;
public:// サブクラス
	struct ConstBufferDataLight
	{
		Vector3 lightv;// ライトへの方向を表すベクトル
		float pad2; // パディング
		Vector3 lightcolor;// ライトの色
	};

private:// 静的メンバ変数
	// デバイス
	static ID3D12Device* device;

public:// 静的メンバ関数
	// 静的初期化
	static void StaticInititalize(ID3D12Device* device);

public:// メンバ関数

	// インスタンス生成
	static Light* Create();

	// 初期化
	void Initialize();

	// 更新処理
	void Update();

	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList);


	// セッター
	// ライト方向をセット
	void SetLightDir(const Vector3& lightdir);

	// ライトの色をセット
	void SetLightColor(const Vector3& lightcolor);

private:// プライベートメンバ関数

	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// マッピングする
	/// </summary>
	void Map();

	/// <summary>
	/// 行列の転送
	/// </summary>
	void TranferConstBuffer();
private:// メンバ変数
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff;

	// ライト光線方向（単位ベクトル）
	Vector3 lightdir = { 1,0,1 };
	// ライトの色
	Vector3 lightcolor = { 1.0f,1.0f,1.0f };
	// ダーティフラグ
	bool dirty = false;


};

