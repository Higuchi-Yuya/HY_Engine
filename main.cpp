#pragma region �ǂݍ��ރw�b�_�[

#pragma comment(lib,"d3dcompiler.lib")

#include<iostream>

#include <math.h>

#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "SpriteManager.h"
#include "Sprite.h"
#include "Object3d.h"

#pragma endregion

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
	spriteManager->Initialize(dxCommon);
	
	// �e�N�X�`���̏�����
	Texture::StaticInitialize(dxCommon);
	
	// �I�u�W�F�N�g�̏�����
	Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_height);

	/////////////////////////////////////////////////////////
	//--------------DirectX12�����������@�����܂�-------------//
	///////////////////////////////////////////////////////
#pragma endregion

#pragma region �V�[���Ɏg���ϐ��̏�����
	// �X�v���C�g�ꖇ�̏�����
	int textureHandle;
	int textureHandle2;
	textureHandle = Texture::LoadTexture("risu.jpg");
	textureHandle2 = Texture::LoadTexture("texture.png");
	Sprite* sprite = new Sprite();
	Sprite* sprite2 = new Sprite();
	sprite->StaticInitialize(spriteManager);
	sprite->Initialize(textureHandle,{100,100});
	sprite2->Initialize(textureHandle2, { 200,200 });

	Object3d* object3d = Object3d::Create();
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
		
		object3d->Update();


		//////////////////////////////////////////////
		//-------DireceX���t���[�������@�����܂�--------//
		////////////////////////////////////////////

#pragma endregion

#pragma region �`�揈��

		//�`��R�}���h��������
		dxCommon->PreDraw();
		
#pragma region �w�i�X�v���C�g�`��
		// �w�i�X�v���C�g�`��
		//-----�������� �w�i�X�v���C�g�`�� -----//


		//-----�����܂� �w�i�X�v���C�g�`�� -----//
		//// �[�x�o�b�t�@�N���A
		dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region �R�c���f���`��
		Object3d::PreDraw(dxCommon->GetCommandList());
		//-----�������� 3D���f���̕`�� -----//
		
		object3d->Draw();

		
		//-----�����܂� 3D���f���̕`�� -----//
		Object3d::PostDraw();
#pragma endregion
		
#pragma region �O�i�X�v���C�g�`��
		// �`��O����
		spriteManager->PreDraw();
		//-----�������� 2D�`�� -------//
		sprite->Draw();
		sprite2->Draw();



		//-----�����܂� 2D�`�� -------//
		// �`��㏈��
		spriteManager->PostDraw();
#pragma endregion

		// �`��R�}���h�̏I��
		dxCommon->PostDraw();



#pragma endregion

	}
#pragma region  WindowsAPI��n��

	//�����N���X�͎g��Ȃ��̂œo�^����������
	delete sprite;
	delete spriteManager;
	delete object3d;

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
