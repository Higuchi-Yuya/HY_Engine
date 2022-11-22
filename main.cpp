#pragma region �ǂݍ��ރw�b�_�[

#pragma comment(lib,"d3dcompiler.lib")


#include<iostream>

#include <math.h>

#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "SpriteManager.h"
#include "Sprite.h"

using namespace Microsoft::WRL;
using namespace std;


#pragma endregion

const float PI = 3.141592f;

#pragma region ���܂��Ȃ�
// @brief �R���\�[����ʂɃt�H�[�}�b�g�t��������̕\��
// @param format �t�H�[�}�b�g(%d�Ƃ�%f�Ƃ���)
// @param �ϒ�����
// @remarks ���̊֐��̓f�o�b�N�p�ł��B�f�o�b�O���ɂ������삵�܂���
void DebugOutputFormatString(const char* format, ...) {
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif
}


#pragma endregion

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR,  _In_ int) {

#pragma region WindowsAPI����������
	// �|�C���^
	WinApp* winApp = nullptr;

	// WindouwsAPI�̏�����
	winApp = new WinApp();
	winApp->Initialize();

#pragma endregion

#pragma region DirectX����������

	////////////////////////////////////////////////////
	//-------------DirectX12������������������-----------//
	//////////////////////////////////////////////////

	// Common�|�C���^
	DirectXCommon* dxCommon = nullptr;

	// DirectX�̏�����
	dxCommon = new DirectXCommon();
	dxCommon->Initalize(winApp);

	// �|�C���^
	Input* input = nullptr;

	// ���͂̏�����
	input = new Input();
	input->Initialize(winApp);

	// �X�v���C�g�̏�����
	SpriteManager* spriteManager = nullptr;
	// �X�v���C�g���ʕ��̏�����
	spriteManager = new SpriteManager;
	spriteManager->Initialize(dxCommon->GetDevice());
	
	/////////////////////////////////////////////////////////
	//--------------DirectX12�����������@�����܂�-------------//
	///////////////////////////////////////////////////////
#pragma endregion

#pragma region �V�[���Ɏg���ϐ��̏�����
	// �X�v���C�g�ꖇ�̏�����
	Sprite* sprite = new Sprite();
	sprite->Initialize();
#pragma endregion


	
	//�Q�[�����[�v
	while (true) {
#pragma region �E�B���h�E���b�Z�[�W����

		// Windows�̃��b�Z�[�W����
		if (winApp->ProcessMessage()) {
			// �Q�[�����[�v�𔲂���
			break;
		}

#pragma endregion

#pragma region DirectX���t���[������
		/////////////////////////////////////////////////////
		//----------DireceX���t���[�������@��������------------//
		///////////////////////////////////////////////////

		// ���͂̍X�V
		input->Update();
		
		// ������0�L�[��������Ă�����
		if (input->PushKey(DIK_0))
		{
			OutputDebugStringA("Hit 0\n");  // �o�̓E�B���h�E�ɁuHit 0�v�ƕ\��
		}

		

		

		//2.�`���̕ύX
		//�����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
		
		

		

		

		////�g���K�[����
		//bool �L�[����������Ԃ�(uint8_t �L�[�ԍ�);
		//bool �L�[�𗣂�����Ԃ�(uint8_t �L�[�ԍ�);
		//bool �L�[���������u�Ԃ�(uint8_t �L�[�ԍ�);
		//bool �L�[�𗣂����u�Ԃ�(uint8_t �L�[�ԍ�);






		
		



		//////////////////////////////////////////////
		//-------DireceX���t���[�������@�����܂�--------//
		////////////////////////////////////////////

#pragma endregion

#pragma region �O���t�B�b�N�X�R�}���h

		//4.�`��R�}���h��������
		dxCommon->PreDraw();
		// �`��O����

		// �`��㏈��
	
		//// �[�x�o�b�t�@�N���A
		dxCommon->ClearDepthBuffer();
		//Mesh�̕`��--------------------------------------------------------------//
		
		
		//
		//for (int i = 0; i < 20; i++)
		//{
		//	line[i].Line_Draw(indices_count2, dxCommon->GetCommandList());
		//}
		//4.�`��R�}���h�����܂�
		
#pragma region �O�i�X�v���C�g�`��
// �`��O����
		

		

		// �`��㏈��
	
#pragma endregion


		

#pragma region ��ʓ���ւ�
		dxCommon->PostDraw();
#pragma endregion


#pragma endregion




	}
#pragma region  WindowsAPI��n��

	//�����N���X�͎g��Ȃ��̂œo�^����������
	delete sprite;
	delete spriteManager;
	// ���͉��
	delete input;

	// WindouwsAPI�̏I������
	winApp->Finalize();

	// WindouwsAPI���
	delete winApp;

	// DirectXcommon�̉��
	delete dxCommon;
#pragma endregion

	//Render_basic::DestroyInstance();

	return 0;
}
