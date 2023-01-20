#pragma once
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "CircleShadow.h"
#include <d3d12.h>
#include <wrl.h>
class LightGroup
{
private:
	// namespaceの省略
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

public:// 定数
	// 平行光源の数
	static const int DirLightNum = 3;
	// 点光源の数
	static const int PointLightNum = 3;
	// スポットライトの数
	static const int SpotLightNum = 3;
	// 丸影の数
	static const int CircleShadowNum = 1;

public:// サブクラス
	struct ConstBufferData
	{
		// 環境光の色
		Vector3 ambientColor;
		float pad1;
		// 平行光源用
		DirectionalLight::ConstBufferData dirLights[DirLightNum];
		// 点光源用
		PointLight::ConstBufferData pointLights[PointLightNum];
		// スポットライト用
		SpotLight::ConstBufferData spotLights[SpotLightNum];
		// 丸影用
		CircleShadow::ConstBufferData circleShadows[CircleShadowNum];
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

	/// <summary>
	/// 点光源の有効フラグをセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="active">有効フラグ</param>
	void SetPointLightActive(int index, bool active);

	/// <summary>
	/// 点光源のライト座標をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightpos">ライト座標</param>
	void SetPointLightPos(int index, const Vector3& lightpos);

	/// <summary>
	/// 点光源のライト色をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightcolor">ライト色</param>
	void SetPointLightColor(int index, const Vector3& lightcolor);

	/// <summary>
	/// 点光源のライト距離減衰係数をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightatten">ライト距離減衰係数</param>
	void SetPointLightAtten(int index, const Vector3& lightAtten);

	/// <summary>
	/// スポットライトの有効フラグをセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="active">有効フラグ</param>
	void SetSpotLightActive(int index, bool active);

	/// <summary>
	/// スポットライトのライト方向をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightdir">ライト方向</param>
	void SetSpotLightDir(int index, const Vector3& lightdir);

	/// <summary>
	/// スポットライトのライト座標をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightpos">ライト座標</param>
	void SetSpotLightPos(int index, const Vector3& lightpos);

	/// <summary>
	/// スポットライトのライト色をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightcolor">ライト色</param>
	void SetSpotLightColor(int index, const Vector3& lightcolor);

	/// <summary>
	/// スポットライトのライト距離減衰係数をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightatten">ライト距離減衰係数</param>
	void SetSpotLightAtten(int index, const Vector3& lightAtten);

	/// <summary>
	/// スポットライトのライト減衰角度をセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="lightFactorAngle">x:減衰開始角度 y:減衰終了角度</param>
	void SetSpotLightFactorAngle(int index, const Vector2& lightFactorAngle);

	/// <summary>
	/// 丸影の有効フラグをセット
	/// </summary>
	/// <param name="index">ライト番号</param>
	/// <param name="active">有効フラグ</param>
	void SetCircleShadowActive(int index, bool active);

	/// <summary>
	/// 丸影のキャスター座標をセット
	/// </summary>
	/// <param name="index">番号</param>
	/// <param name="lightpos">キャスター座標</param>
	void SetCircleShadowCasterPos(int index, const Vector3& casterPos);

	/// <summary>
	/// 丸影の方向をセット
	/// </summary>
	/// <param name="index">番号</param>
	/// <param name="lightdir">方向</param>
	void SetCircleShadowDir(int index, const Vector3& lightdir);

	/// <summary>
	/// 丸影のキャスターとライトの距離をセット
	/// </summary>
	/// <param name="index">番号</param>
	/// <param name="distanceCasterLight">キャスターとライトの距離</param>
	void SetCircleShadowDistanceCasterLight(int index, float distanceCasterLight);

	/// <summary>
	/// 丸影の距離減衰係数をセット
	/// </summary>
	/// <param name="index">番号</param>
	/// <param name="lightatten">距離減衰係数</param>
	void SetCircleShadowAtten(int index, const Vector3& lightAtten);

	/// <summary>
	/// 丸影の減衰角度をセット
	/// </summary>
	/// <param name="index">番号</param>
	/// <param name="lightFactorAngle">x:減衰開始角度 y:減衰終了角度</param>
	void SetCircleShadowFactorAngle(int index, const Vector2& lightFactorAngle);

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
	DirectionalLight dirLights_[DirLightNum];

	// 点光源の配列
	PointLight pointLights_[PointLightNum];

	// スポットライトの配列
	SpotLight spotLights_[SpotLightNum];

	// 丸影の配列
	CircleShadow circleShadows_[CircleShadowNum];

	// ダーティフラグ
	bool dirty = false;

};

