#pragma once

#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

class PointLight
{
public: // サブクラス

// 定数バッファ用データ構造体
	struct ConstBufferData
	{
		Vector3 lightpos;
		float pad1;
		Vector3 lightcolor;
		float pad2;
		Vector3 lightatten;
		float lihgtIndensity;
		float lightRadius;
		float lightDecay;
		unsigned int active;
		float pad3;
	};

public: // メンバ関数
	/// <summary>
	/// ライト座標をセット
	/// </summary>
	/// <param name="lightpos">ライト座標</param>
	inline void SetLightPos(const Vector3& lightpos) { lightpos_ = lightpos; }

	/// <summary>
	/// ライト座標を取得
	/// </summary>
	/// <returns>ライト座標</returns>
	inline const Vector3& GetLightPos() { return lightpos_; }

	/// <summary>
	/// ライト色をセット
	/// </summary>
	/// <param name="lightcolor">ライト色</param>
	inline void SetLightColor(const Vector3& lightcolor) { lightcolor_ = lightcolor; }

	/// <summary>
	/// ライト色を取得
	/// </summary>
	/// <returns>ライト色</returns>
	inline const Vector3& GetLightColor() { return lightcolor_; }

	/// <summary>
	/// ライト距離減衰係数をセット
	/// </summary>
	/// <param name="lightatten">ライト距離減衰係数</param>
	inline void SetLightAtten(const Vector3& lightAtten) { lightAtten_ = lightAtten; }

	/// <summary>
	/// ライト距離減衰係数を取得
	/// </summary>
	/// <returns>ライト距離減衰係数</returns>
	inline const Vector3& GetLightAtten() { return lightAtten_; }

	/// <summary>
	/// 有効フラグをセット
	/// </summary>
	/// <param name="active">有効フラグ</param>
	inline void SetActive(bool active) { active_ = active; }

	/// <summary>
	/// 有効チェック
	/// </summary>
	/// <returns>有効フラグ</returns>
	inline bool IsActive() { return active_; }

	/// <summary>
	/// 光の輝度をセット
	/// </summary>
	/// <param name="indensity">光から照射点までの距離</param>
	/// <returns></returns>
	inline void SetIndensity(float indensity) { lightIndensity_ = indensity; }

	/// <summary>
	/// 光の輝度を取得
	/// </summary>
	/// <returns></returns>
	inline float GetIndensity() { return lightIndensity_; }

	/// <summary>
	/// 光の届く最大距離をセット
	/// </summary>
	/// <param name="radius">光の強さが最大強度の半分になる距離</param>
	/// <returns></returns>
	inline void SetRadius(float radius) { lightRadius_ = radius; }

	/// <summary>
	/// 光の届く最大距離を取得
	/// </summary>
	/// <returns></returns>
	inline float GetRadius() { return lightRadius_; }

	/// <summary>
	/// 光の減衰度をセット
	/// </summary>
	/// <param name="maxIdensity">光の最大強度</param>
	/// <returns></returns>
	inline void SetDecay(float decay) { lightDecay_ = decay; }

	/// <summary>
	/// 光の減衰度を取得
	/// </summary>
	/// <returns></returns>
	inline float GetDecay() { return lightDecay_; }

	/// <summary>
	/// 光から照射点の距離
	/// </summary>
	/// <param name="distance"></param>
	inline void SetDistance(float distance) { lightDistance_ = distance; }

	/// <summary>
	/// 光から照射点の距離を取得
	/// </summary>
	/// <returns></returns>
	inline float GetDistance() { return lightDistance_; }

private: // メンバ変数
	// ライト座標（ワールド座標系）
	Vector3 lightpos_ = { 0,0,0 };
	// ライト色
	Vector3 lightcolor_ = { 1,1,1 };
	// ライト距離減衰係数
	Vector3 lightAtten_ = { 1.0f, 1.0f, 1.0f };
	// 有効フラグ
	bool active_ = false;

	// 光の輝度
	float lightIndensity_ = 4;
	// 光の届く最大距離
	float lightRadius_ = 3.5f;
	// 光の減衰度
	float lightDecay_ = 1;
	// 光から照射点の距離
	float lightDistance_ = 1;
};

