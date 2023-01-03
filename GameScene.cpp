#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	// ���͉��
	delete input;
	// ���C�g�̉��
	delete light;
	// �X�v���C�g�̉��
	delete sprite;
	delete sprite2;
	// �I�u�W�F�N�g�̉��
	delete obj_2;
	// ���f���̉��
	delete model_2;
	// �r���[�̉��
	delete view;

	//delete fruitsPop;

	// �v���C���[�̉��
	delete player;


	
}

void GameScene::Initialize()
{
	input = new Input();
	input->Initialize();

	// ���C�g�̐���
	light = Light::Create();
	// ���C�g�̐F��ݒ�
	light->SetLightDir({ 0,-1,5 });
	// 3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g
	Object3d::SetLight(light);

	// �e�N�X�`���̏�����
	textureHandle = Texture::LoadTexture("skydome/Nebura.jpg");
	textureHandle2 = Texture::LoadTexture("texture.png");

	// �X�v���C�g�̏�����
	sprite = new Sprite();
	sprite2 = new Sprite();

	sprite->Initialize(textureHandle, { WinApp::window_width / 2,WinApp::window_height / 2 }, { 1280,720 });
	sprite2->Initialize(textureHandle2, { 200,200 });

	// �r���[�v���W�F�N�V�����̏�����
	view = new ViewProjection;
	view->DebugCameraInitialze(input);

	// �v���C���[�̏�����
	player = new Player();
	player->Initialize();

	// �t���[�c�̏�����
	fruitsPop = new FruitsPop();
	fruitsPop->Initialize();


}

void GameScene::Update()
{
	// ���͂̍X�V
	input->Update();

	// ������0�L�[��������Ă�����
	if (input->PushKey(DIK_0))
	{
		OutputDebugStringA("Hit 0\n");  // �o�̓E�B���h�E�ɁuHit 0�v�ƕ\��
	}

	light->Update();

	//view->DebugCameraUpdate();
	view->UpdateMatrix();
	player->Update();



	fruitsPop->SetPlayer(player);
	fruitsPop->Update();

	if (input->TriggerKey(DIK_SPACE)) {
		fruitsPop->Initialize();
	}

}

void GameScene::Draw2Dback()
{
	sprite->Draw();
}

void GameScene::Draw3D()
{
	player->Draw(view);
	fruitsPop->Draw(*view);
}

void GameScene::Draw2Dfront()
{

	sprite2->Draw();
}
