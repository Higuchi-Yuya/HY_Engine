#include "Input.h"

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

#include <cassert>

WinApp* Input::winApp_ = nullptr;

void Input::StaticInitialize(WinApp* winApp)
{
	// �؂�Ă���WinApp�̃C���X�^���X���L�^
	Input::winApp_ = winApp;
}

void Input::Initialize()
{
	HRESULT result;

	//DirectInput�̏�����
	result = DirectInput8Create(winApp_->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput_, nullptr);
	assert(SUCCEEDED(result));

	//�L�[�{�[�h�f�o�C�X�̐���
	result = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(result));

	// �}�E�X�f�o�C�X�̐���
	result = directInput_->CreateDevice(GUID_SysMouse, &devMouse_, NULL);
	assert(SUCCEEDED(result));

	// �L�[�{�[�h���̓f�[�^�`���̃Z�b�g
	result = keyboard_->SetDataFormat(&c_dfDIKeyboard);//�W���`��
	assert(SUCCEEDED(result));

	// �L�[�{�[�h�r�����䃌�x���̃Z�b�g
	result = keyboard_->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));

	// �}�E�X���̓f�[�^�`���̃Z�b�g
	result = devMouse_->SetDataFormat(&c_dfDIMouse2); // �W���`��
	assert(SUCCEEDED(result));

	// �}�E�X�r�����䃌�x���̃Z�b�g
	result = devMouse_->SetCooperativeLevel(winApp_->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
}

void Input::Update()
{
	// �O��̃L�[���͂�ۑ�
	memcpy(keyPre_, key_, sizeof(key_));

	// �L�[�{�[�h���̎擾�J�n
	keyboard_->Acquire();

	// �S�L�[�̓��͏�Ԃ��擾����
	keyboard_->GetDeviceState(sizeof(key_), key_);

	// �}�E�X����J�n
	devMouse_->Acquire(); 

	// �O��̓��͂�ۑ�
	mouseStatePre_ = mouseState_;

	// �}�E�X�̓���
	devMouse_->GetDeviceState(sizeof(mouseState_), &mouseState_);
}

bool Input::PushKey(BYTE keyNumber)
{
	// �w�肵���L�[�������Ă����true��Ԃ�
	if (key_[keyNumber]) {
		return true;
	}

	// �����łȂ����false��Ԃ�
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	// �w��L�[��O�t���[���ŉ����Ă��Ȃ��A���̃t���[���ŉ����Ă����true��Ԃ�
	if (!keyPre_[keyNumber] && key_[keyNumber]) {
		return true;
	}

	// �����łȂ����false��Ԃ�
	return false;
}

bool Input::ReleasedKey(BYTE keyNumber)
{
	// �w��L�[��O�t���[���ŉ����Ă��āA���̃t���[���ŉ����Ă��Ȃ����true��Ԃ�
	if (keyPre_[keyNumber] && !key_[keyNumber]) {
		return true;
	}

	// �����łȂ����false��Ԃ�
	return false;
}

bool Input::PushMouseLeft()
{
	// 0�łȂ���Ή����Ă���
	if (mouseState_.rgbButtons[0]) {
		return true;
	}

	// �����Ă��Ȃ�
	return false;
}

bool Input::PushMouseMiddle()
{
	// 0�łȂ���Ή����Ă���
	if (mouseState_.rgbButtons[2]) {
		return true;
	}

	// �����Ă��Ȃ�
	return false;
}

bool Input::TriggerMouseLeft()
{
	// �O��0�ŁA����0�łȂ���΃g���K�[
	if (!mouseStatePre_.rgbButtons[0] && mouseState_.rgbButtons[0]) {
		return true;
	}

	// �g���K�[�łȂ�
	return false;
}

bool Input::TriggerMouseMiddle()
{
	// �O��0�ŁA����0�łȂ���΃g���K�[
	if (!mouseStatePre_.rgbButtons[2] && mouseState_.rgbButtons[2]) {
		return true;
	}

	// �g���K�[�łȂ�
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
