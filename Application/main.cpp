#pragma region �ǂݍ��ރw�b�_�[

#pragma comment(lib,"d3dcompiler.lib")

#include<iostream>

#include <math.h>

#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "SpriteManager.h"
#include "LightGroup.h"
#include "GameScene.h"
#include "PostEffect.h"
#include "InputManager.h"
#include <dxgidebug.h>
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

struct D3DResouceLeakChecker
{
	~D3DResouceLeakChecker()
	{
		// ���\�[�X���[�N�`�F�b�N
		Microsoft::WRL::ComPtr<IDXGIDebug1>debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};


int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR,  _In_ int) {

	D3DResouceLeakChecker leckCheck;

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

	// ImGui�̏�����
	ImGuiManager* imguiManager = new ImGuiManager();
	imguiManager->Initialize(winApp,dxCommon);

	// ���͂̏�����
	Input::StaticInitialize(winApp);

	InputManager* inputManager = InputManager::GetInstance().get();
	inputManager->SetWinApp(winApp);
	inputManager->Init();

	// �X�v���C�g�̏�����
	std::unique_ptr<SpriteManager> spriteManager = nullptr;
	// �X�v���C�g���ʕ��̏�����
	spriteManager = std::make_unique<SpriteManager>();
	spriteManager->Initialize(dxCommon);
	
	Sprite::StaticInitialize(spriteManager.get());

	// �e�N�X�`���̏�����
	TextureManager::StaticInitialize(dxCommon);
	
	// �I�u�W�F�N�g�̏�����
	Object3d::StaticInitialize(dxCommon->GetDevice());

	// �r���[�v���W�F�N�V�����̏�����
	ViewProjection::StaticInitialize(dxCommon->GetDevice());

	// �T�E���h�̐ÓI�̏�����
	Sound::StaticInitialize();

	// ���C�g�̐ÓI������
	LightGroup::StaticInititalize(dxCommon->GetDevice());

	// �t�H�O�̐ÓI������
	Fog::StaticInitialize(dxCommon->GetDevice());

	// FBX�̏�����
	FbxModel::SetDevice(dxCommon->GetDevice());
	FbxModel::StaticInitialize();

	// �|�X�g�G�t�F�N�g�̏�����
	PostEffect::SetDevice(dxCommon->GetDevice());

	/////////////////////////////////////////////////////////
	//--------------DirectX12�����������@�����܂�-------------//
	///////////////////////////////////////////////////////
#pragma endregion

#pragma region �V�[���Ɏg���ϐ��̏�����
	GameScene* gameScene = new GameScene();
	gameScene->Initialize();
	gameScene->SetDxComon(dxCommon);

	//PostEffect* postEffect = nullptr;
	//// ����Ɏ���������
	//PostColorInversion* post = nullptr;
	//Texture tex = TextureManager::Load2DTexture("risu.jpg");

	//postEffect = new PostEffect();
	//postEffect->Initialize();

	//post = new PostColorInversion();
	//post->Initialize();


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

		InputManager::GetInstance()->Update();
		


		//////////////////////////////////////////////
		//-------DireceX���t���[�������@�����܂�--------//
		////////////////////////////////////////////
#pragma region IMGUI�̍X�V����
	// ImGui�̍X�V����
		imguiManager->Begin();
		gameScene->Update();
		gameScene->ImguiUpdate();
		
		imguiManager->End();
#pragma endregion

#pragma endregion

		//postEffect->PreDrawScene(dxCommon->GetCommandList());

		//postEffect->PostDrawScene(dxCommon->GetCommandList());

		//post->PreDrawScene(dxCommon->GetCommandList());
		//
		//postEffect->Draw(dxCommon->GetCommandList());

		//post->PostDrawScene(dxCommon->GetCommandList());

#pragma region �`�揈��



		//�`��R�}���h��������
		dxCommon->PreDraw();
		
#pragma region �w�i�X�v���C�g�`��
		// �w�i�X�v���C�g�`��

		spriteManager->PreDraw();
		//-----�������� �w�i�X�v���C�g�`�� -----//
		gameScene->Draw2DBack();

		//post->Draw(dxCommon->GetCommandList());

		//-----�����܂� �w�i�X�v���C�g�`�� -----//
		spriteManager->PostDraw();
		// �[�x�o�b�t�@�N���A
		dxCommon->ClearDepthBuffer();
#pragma endregion

#pragma region �R�c���f���`��
		Object3d::PreDraw(dxCommon->GetCommandList());
		//-----�������� 3D���f���̕`�� -----//
		gameScene->Draw3D();

		
		//-----�����܂� 3D���f���̕`�� -----//
		Object3d::PostDraw();
#pragma endregion
		
#pragma region �O�i�X�v���C�g�`��
		// �`��O����
		spriteManager->PreDraw();
		//-----�������� 2D�`�� -------//
		gameScene->Draw2DFront();
		


		//-----�����܂� 2D�`�� -------//
		// �`��㏈��
		spriteManager->PostDraw();
#pragma endregion

#pragma region IMGUI�̕`��
		imguiManager->Draw();
#pragma endregion


		// �`��R�}���h�̏I��
		dxCommon->PostDraw();

#pragma endregion
	}
#pragma region  WindowsAPI��n��

	//�����N���X�͎g��Ȃ��̂œo�^����������

	// �Q�[���V�[���̉��
	delete gameScene;
	// ImGui�̃}�l�[�W���[�����
	imguiManager->Finalize();
	delete imguiManager;

	TextureManager::StaticFinalize();
	Object3d::StaticFinalize();
	FbxModel::StaticFainalize();

	// WindouwsAPI�̏I������
	winApp->Finalize();
	// WindouwsAPI���
	delete winApp;
	// DirectXcommon�̉��
	delete dxCommon;
#pragma endregion

	return 0;
}
