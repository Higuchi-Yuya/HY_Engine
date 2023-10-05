#include "Input.h"


#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include <cassert>

WinApp* Input::winApp_ = nullptr;

void Input::StaticInitialize(WinApp* winApp)
{
	// 借りてきたWinAppのインスタンスを記録
	Input::winApp_ = winApp;
}

void Input::Initialize()
{
	HRESULT result;

	//DirectInputの初期化
	result = DirectInput8Create(winApp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	// マウスデバイスの生成
	result = directInput_->CreateDevice(GUID_SysMouse, &devMouse_, NULL);
	assert(SUCCEEDED(result));

	// キーボード入力データ形式のセット
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard);//標準形式
	assert(SUCCEEDED(result));

	// キーボード排他制御レベルのセット
	result = keyboard_->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	// マウス入力データ形式のセット
	result = devMouse_->SetDataFormat(&c_dfDIMouse2); // 標準形式
	assert(SUCCEEDED(result));

	// マウス排他制御レベルのセット
	result = devMouse_->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update()
{
	// 前回のキー入力を保存
	memcpy(keyPre_, key_, sizeof(key_));

	// キーボード情報の取得開始
	keyboard_->Acquire();

	// 全キーの入力状態を取得する
	keyboard_->GetDeviceState(sizeof(key_), key_);

	// マウス動作開始
	devMouse_->Acquire(); 

	// 前回の入力を保存
	mouseStatePre_ = mouseState_;

	// マウスの入力
	devMouse_->GetDeviceState(sizeof(mouseState_), &mouseState_);
}

bool Input::PushKey(BYTE keyNumber)
{
	// 指定したキーを押していればtrueを返す
	if (key_[keyNumber]) {
		return true;
	}

	// そうでなければfalseを返す
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	// 指定キーを前フレームで押していなく、今のフレームで押していればtrueを返す
	if (!keyPre_[keyNumber] && key_[keyNumber]) {
		return true;
	}

	// そうでなければfalseを返す
	return false;
}

bool Input::ReleasedKey(BYTE keyNumber)
{
	// 指定キーを前フレームで押していて、今のフレームで押していなければtrueを返す
	if (keyPre_[keyNumber] && !key_[keyNumber]) {
		return true;
	}

	// そうでなければfalseを返す
	return false;
}

bool Input::PushMouseLeft()
{
	// 0でなければ押している
	if (mouseState_.rgbButtons[0]) {
		return true;
	}

	// 押していない
	return false;
}

bool Input::PushMouseMiddle()
{
	// 0でなければ押している
	if (mouseState_.rgbButtons[2]) {
		return true;
	}

	// 押していない
	return false;
}

bool Input::TriggerMouseLeft()
{
	// 前回が0で、今回が0でなければトリガー
	if (!mouseStatePre_.rgbButtons[0] && mouseState_.rgbButtons[0]) {
		return true;
	}

	// トリガーでない
	return false;
}

bool Input::TriggerMouseMiddle()
{
	// 前回が0で、今回が0でなければトリガー
	if (!mouseStatePre_.rgbButtons[2] && mouseState_.rgbButtons[2]) {
		return true;
	}

	// トリガーでない
	return false;
}

Input::MouseMove Input::GetMouseMove()
{
	MouseMove tmp;
	tmp.lX = mouseState_.lX;
	tmp.lY = mouseState_.lY;
	tmp.lZ = mouseState_.lZ;
	return tmp;
}
