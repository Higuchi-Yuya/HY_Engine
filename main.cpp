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
#include "ViewProjection.h"
#include "Model.h"
#include "Quaternion.h"
#include "Light.h"

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

	// �r���[�v���W�F�N�V�����̏�����
	ViewProjection::StaticInitialize(dxCommon->GetDevice());

	// ���C�g�̐ÓI������
	Light::StaticInititalize(dxCommon->GetDevice());

	// �I�u�W�F�N�g���ʂ̃��C�g�̏�����
	Light* light = nullptr;

	// ���C�g�̐���
	light = Light::Create();
	// ���C�g�̐F��ݒ�
	light->SetLightDir({ 1,0,0 });
	// 3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g
	Object3d::SetLight(light);

	/////////////////////////////////////////////////////////
	//--------------DirectX12�����������@�����܂�-------------//
	///////////////////////////////////////////////////////
#pragma endregion

#pragma region �V�[���Ɏg���ϐ��̏�����
	// �X�v���C�g�ꖇ�̏�����
	int textureHandle;
	int textureHandle2;
	textureHandle = Texture::LoadTexture("skydome/Nebura.jpg");
	textureHandle2 = Texture::LoadTexture("texture.png");
	Sprite* sprite = new Sprite();
	Sprite* sprite2 = new Sprite();
	sprite->StaticInitialize(spriteManager);
	sprite->Initialize(textureHandle, { WinApp::window_width / 2,WinApp::window_height / 2 }, { 1280,720 });
	sprite2->Initialize(textureHandle2, { 200,200 });

	Model* model = Model::LoadFromOBJ("skydome",true);
	Model* model_2 = Model::LoadFromOBJ("Medama",true);

	//Object3d* object3d = Object3d::Create();
	Object3d* obj_2 = Object3d::Create();
	
	//object3d->SetModel(model);
	//object3d->worldTransform_.position_ = { 10,0,0, };
	//object3d->worldTransform_.scale_ = { 5.0f,5.0f,5.0f };
	obj_2->SetModel(model_2);
	obj_2->worldTransform_.scale_ = { 5.0f,5.0f,5.0f };
	
	ViewProjection* view = new ViewProjection;
	view->DebugCameraInitialze(input);
	

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
		
		light->Update();
		//object3d->SetScale(scale_);
		//object3d->Update();
		obj_2->worldTransform_.rotation_.y += 0.01f;
		obj_2->Update();
		view->DebugCameraUpdate();

		//////////////////////////////////////////////
		//-------DireceX���t���[�������@�����܂�--------//
		////////////////////////////////////////////

#pragma endregion

#pragma region �`�揈��

		//�`��R�}���h��������
		dxCommon->PreDraw();
		
#pragma region �w�i�X�v���C�g�`��
		// �w�i�X�v���C�g�`��
		spriteManager->PreDraw();
		//-----�������� �w�i�X�v���C�g�`�� -----//
		sprite->Draw();



		//-----�����܂� �w�i�X�v���C�g�`�� -----//
		spriteManager->PostDraw();
		// �[�x�o�b�t�@�N���A
		dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region �R�c���f���`��
		Object3d::PreDraw(dxCommon->GetCommandList());
		//-----�������� 3D���f���̕`�� -----//
		
		//object3d->Draw(view);
		obj_2->Draw(view);
		
		//-----�����܂� 3D���f���̕`�� -----//
		Object3d::PostDraw();
#pragma endregion
		
#pragma region �O�i�X�v���C�g�`��
		// �`��O����
		spriteManager->PreDraw();
		//-----�������� 2D�`�� -------//
		//sprite->Draw();
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
	// �X�v���C�g�̉��
	delete sprite;
	delete sprite2;
	// �I�u�W�F�N�g�̉��
	//delete object3d;
	delete obj_2;
	// ���f���̉��
	delete model;
	delete model_2;
	// �r���[�̉��
	delete view;
	// ���C�g�̉��
	delete light;

	// �X�v���C�g�}�l�[�W���[�̉��
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

	return 0;
}
