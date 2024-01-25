#pragma once

#include "Vector3.h"
#include <Vector2.h>

//追加
class Easing
{
public://サブクラス
	enum EaseType
	{
		_In,
		_Out,
		_InOut,
		_InBack,
		_OutBack,
		_InOutBack,
		_OutBounce,
		_InBounce,
		_InOutBounce,
		_InCirc,
		_OutCubic_
	};

public:
	Easing();

	Easing(const int32_t limitTime);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 変数リセット
	/// </summary>
	void Reset();

public:// セッター

	/// <summary>
	/// イージングのリミットタイムを設定
	/// </summary>
	/// <param name="limitTime">リミットタイム</param>
	void SetEaseLimitTime(const int32_t limitTime);

	/// <summary>
	/// イージングのタイプを設定
	/// </summary>
	/// <param name="easeType">イージングのタイプ</param>
	void SetEaseType(const EaseType easeType);

	/// <summary>
	/// イージングの終了しているかを設定
	/// </summary>
	/// <param name="isEnd">イージングの終了フラグ</param>
	void SetIsEnd(const bool isEnd);

public:// ゲッター

	/// <summary>
	///	タイマーを取得
	/// </summary>
	/// <returns></returns>
	int32_t GetTimer();

	/// <summary>
	/// リミットタイムを取得
	/// </summary>
	/// <returns></returns>
	int32_t GetLimitTime();

	/// <summary>
	/// イージングのタイプを取得
	/// </summary>
	/// <returns></returns>
	EaseType GetEaseType();

	/// <summary>
	/// イージングの終了フラグを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsEnd();
public:
	float Lerp(const float startPos, const float endPos);

	float In(float start, float end);

	float Out(float start, float end);
	Vector3 OutVec3(Vector3 start, Vector3 end);

	float InOut(float start, float end);
	Vector3 InOut(Vector3 start, Vector3 end);

	float In_Back(float start, float end);

	float Out_Back(float start, float end);

	float InOut_Back(float start, float end);

	float Out_Bounce(float start, float end);

	float In_Bounce(float start, float end);

	float InOut_Bounce(float start, float end);

	float easeOutCubic(float start, float end);
	Vector3 easeOutCircVec3(Vector3 start, Vector3 end);

	float easeInCirc(float start, float end);
	Vector2 easeInCircVec2(Vector2 start, Vector2 end);
	Vector3 easeInCircVec3(Vector3 start, Vector3 end);

private:// メンバ
	// 時間
	int32_t timer_;
	// 最大時間
	int32_t limitTimer_;
	// タイムレイト
	float timeRate_;

	// 終わっているかフラグ
	bool IsEnd_;

	// イージングのタイプ
	EaseType easeType_;
};
