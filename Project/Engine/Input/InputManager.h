#pragma once

#define DIRECTINPUT_VERSION 0x0800	// DirectInputのバージョン指定
#include "JoypadInput.h"
#include "Input.h"
#include "WinApp.h"
#include <dinput.h>
#include <wrl.h>

template<typename T> class Singleton;

class InputManager : public Singleton<InputManager>
{
private:
	friend Singleton<InputManager>;
	Microsoft::WRL::ComPtr<IDirectInput8> directInput_;
	// 借りてくるWindowsAPI
	static WinApp* winApp_;
public:
	void SetWinApp(WinApp* winApp);
	void Init();
	void Update();

public: // ゲッター
	inline Microsoft::WRL::ComPtr<IDirectInput8> GetDirectInput() { return directInput_; }
};

