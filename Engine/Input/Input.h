#pragma once
#include <windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION 0x0800 //DirectInputのバージョン指定
#include <dinput.h>
#include "WinApp.h"

// 入力
class Input
{
private:
	// namespaceの省略
	template <class T>using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	struct MouseMove {
		LONG lX;
		LONG lY;
		LONG lZ;
	};
public: // メンバ関数
	// 静的初期化
	static void StaticInitialize(WinApp* winApp);

	// 初期化
	void Initialize();

	// 更新
	void Update();

	/// <summary>
	/// キーの押したかをチェック(長押し)
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_0 等)</param>
	/// <returns>押されているか</returns>
	bool PushKey(BYTE keyNumber);

	/// <summary>
	/// キーのトリガーをチェック(押した瞬間)
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_0 等)</param>
	/// <returns>トリガーか</returns>
	bool TriggerKey(BYTE keyNumber);

	/// <summary>
	/// キーのトリガーをチェック(離した瞬間)
	/// </summary>
	/// <param name="keyNumber">キー番号(DIK_0 等)</param>
	/// <returns>トリガーか</returns>
	bool ReleasedKey(BYTE keyNumber);

	/// <summary>
	/// キーの左ボタン押下をチェック
	/// </summary>
	/// <returns>押されているか</returns>
	bool PushMouseLeft();

	/// <summary>
	/// キーの中ボタン押下をチェック
	/// </summary>
	/// <returns>押されているか</returns>
	bool PushMouseMiddle();

	/// <summary>
	/// キーの左ボタントリガーをチェック
	/// </summary>
	/// <returns>トリガーか</returns>
	bool TriggerMouseLeft();

	/// <summary>
	/// キーの中ボタントリガーをチェック
	/// </summary>
	/// <returns>トリガーか</returns>
	bool TriggerMouseMiddle();

	/// <summary>
	/// マウス移動量を取得
	/// </summary>
	/// <returns>マウス移動量</returns>
	MouseMove GetMouseMove();

private:// 静的メンバ変数

	// DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput = nullptr;

	// キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

	// 全キーの状態
	BYTE key[256] = {};

	// 前回の全キーの状態
	BYTE keyPre[256] = {};

	// マウスのデバイス
	ComPtr<IDirectInputDevice8> devMouse;

	// マウスの状態
	DIMOUSESTATE2 mouseState = {};

	// マウスの前の状態
	DIMOUSESTATE2 mouseStatePre = {};

	// WindowsAPI
	static WinApp* winApp_;
};

