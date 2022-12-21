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
	delete object3d;
	delete obj_2;
	// ���f���̉��
	delete model;
	delete model_2;
	// �r���[�̉��
	delete view;
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

	// ���f���̏�����
	model = Model::LoadFromOBJ("sphere", true);
	model_2 = Model::LoadFromOBJ("Medama", true);

	// �I�u�W�F�N�g�̏�����
	object3d = Object3d::Create();
	obj_2 = Object3d::Create();

	object3d->SetModel(model);
	object3d->worldTransform_.position_ = { 10,0,0, };
	object3d->worldTransform_.scale_ = { 5.0f,5.0f,5.0f };
	obj_2->SetModel(model_2);
	obj_2->worldTransform_.scale_ = { 5.0f,5.0f,5.0f };
	obj_2->worldTransform_.color_ = { 1.0f,1.0f,1.0f,0.95f };

	// �r���[�v���W�F�N�V�����̏�����
	view = new ViewProjection;
	view->DebugCameraInitialze(input);
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

	object3d->worldTransform_.rotation_.y += 0.01f;
	object3d->Update();

	obj_2->worldTransform_.rotation_.y += 0.01f;
	obj_2->Update();
	view->DebugCameraUpdate();

}

void GameScene::Draw2Dback()
{
	sprite->Draw();
}

void GameScene::Draw3D()
{
	object3d->Draw(view);
	obj_2->Draw(view);
}

void GameScene::Draw2Dfront()
{

	sprite2->Draw();
}
