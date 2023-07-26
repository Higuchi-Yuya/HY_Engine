#include "InputManager.h"
#include <cassert>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
WinApp* InputManager::winApp_ = nullptr;

void InputManager::SetWinApp(WinApp* winApp)
{
	winApp_ = winApp;
	JoypadInput::SetWinApp(winApp_);
}

void InputManager::Init()
{
	HRESULT result;
	

	// DirectInput‚Ì‰Šú‰»
	result = DirectInput8Create(
		winApp_->GetHInstance(),
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));

	JoypadInput::GetInstance()->Init();
}

void InputManager::Update()
{
	JoypadInput::GetInstance()->Update();
}