#pragma once
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

class DirectionalLight
{
public:// サブクラス
	struct ConstBufferData
	{
		Vector3 lightv;// ライトへの方向を表すベクトル
		float pad1; // パディング
		Vector3 lightcolor;// ライトの色
		unsigned int active;
	};

public:
	DirectionalLight();
	~DirectionalLight();

	/// <summary>
	/// ライト方向をセット
	/// </summary>
	/// <param name="lightdir">ライト方向</param>
	inline void SetLightDir(const Vector3& lightdir){lightdir_ = lightdir;lightdir_.normalize();};

	/// <summary>
	/// ライト方向を取得
	/// </summary>
	/// <returns>ライト方向</returns>
	inline const Vector3& GetLightDir() { return lightdir_; }

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
	/// 有効フラグをセット
	/// </summary>
	/// <param name="active">有効フラグ</param>
	inline void SetActive(bool active) { active_ = active; }

	/// <summary>
	/// 有効チェック
	/// </summary>
	/// <returns>有効フラグ</returns>
	inline bool IsActive() { return active_; }

private:
	// ライト光線方向（単位ベクトル）
	Vector3 lightdir_ = { 1,0,0 };

	// ライトの色
	Vector3 lightcolor_ = { 1.0f,1.0f,1.0f };

	// 有効フラグ
	bool active_ = false;

};
