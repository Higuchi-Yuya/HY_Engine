#pragma once
#include "DirectionalLight.h"
#include <d3d12.h>
#include <wrl.h>
class LightGroup
{
private:
	// namespaceの省略
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

public:// 定数
	// 平行光源の数
	static const int kDirLightNum = 3;
	// 点光源の数
	static const int kPointLightNum = 3;
	// スポットライトの数
	static const int kSpotLightNum = 3;
	// 丸影の数
	static const int kCircleShadowNum = 1;

public:// サブクラス
	struct ConstBufferData
	{
		// 環境光の色
		Vector3 ambientColor;
		float pad1;
		// 平行光源用
		DirectionalLight::ConstBufferData dirLights[kDirLightNum];
	};

private:// 静的メンバ変数
	// デバイス
	static ID3D12Device* device;

public:// 静的メンバ関数
	// 静的初期化
	static void StaticInititalize(ID3D12Device* device);

public:// メンバ関数

	// インスタンス生成
	static LightGroup* Create();

	// 初期化
	void Initialize();

	// 更新処理
	void Update();

	// 描画
	void Draw(ID3D12GraphicsCommandList* cmdList);

	/// <summary>
	/// 環境光のライト色をセット
	/// </summary>
	/// <param name="color">ライト色</param>
	void SetAmbientColor(const Vector3& color);

	/// <summary>
	/// 平行光源の有効フラグをセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="active">有効フラグ</param>
	void SetDirLightActive(int index, bool active);

	/// <summary>
	/// 平行光源のライト方向をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightdir">ライト方向</param>
	void SetDirLightDir(int index, const Vector3& lightdir);

	/// <summary>
	/// 平行光源のライト色をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightcolor">ライト色</param>
	void SetDirLightColor(int index, const Vector3& lightcolor);

private:// プライベートメンバ関数

	/// <summary>
	/// 定数バッファ生成
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// 定数バッファ転送
	/// </summary>
	void TransferConstBuffer();

	/// <summary>
	/// 標準のライト設定
	/// </summary>
	void DefaultLightSetting();

private:// メンバ変数
	// 定数バッファ
	ComPtr<ID3D12Resource> constBuff;
	// 定数バッファのマップ
	ConstBufferData* constMap = nullptr;

	// 環境光の色
	Vector3 ambientColor_ = { 1,1,1 };

	// 平行光源の配列
	DirectionalLight dirLights_[kDirLightNum];


	// ダーティフラグ
	bool dirty = false;

};

