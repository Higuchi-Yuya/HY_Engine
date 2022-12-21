#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	// 入力解放
	delete input;
	// ライトの解放
	delete light;
	// スプライトの解放
	delete sprite;
	delete sprite2;
	// オブジェクトの解放
	delete object3d;
	delete obj_2;
	// モデルの解放
	delete model;
	delete model_2;
	// ビューの解放
	delete view;
}

void GameScene::Initialize()
{
	input = new Input();
	input->Initialize();

	// ライトの生成
	light = Light::Create();
	// ライトの色を設定
	light->SetLightDir({ 0,-1,5 });
	// 3Dオブジェクトにライトをセット
	Object3d::SetLight(light);

	// テクスチャの初期化
	textureHandle = Texture::LoadTexture("skydome/Nebura.jpg");
	textureHandle2 = Texture::LoadTexture("texture.png");

	// スプライトの初期化
	sprite = new Sprite();
	sprite2 = new Sprite();

	sprite->Initialize(textureHandle, { WinApp::window_width / 2,WinApp::window_height / 2 }, { 1280,720 });
	sprite2->Initialize(textureHandle2, { 200,200 });

	// モデルの初期化
	model = Model::LoadFromOBJ("sphere", true);
	model_2 = Model::LoadFromOBJ("Medama", true);

	// オブジェクトの初期化
	object3d = Object3d::Create();
	obj_2 = Object3d::Create();

	object3d->SetModel(model);
	object3d->worldTransform_.position_ = { 10,0,0, };
	object3d->worldTransform_.scale_ = { 5.0f,5.0f,5.0f };
	obj_2->SetModel(model_2);
	obj_2->worldTransform_.scale_ = { 5.0f,5.0f,5.0f };
	obj_2->worldTransform_.color_ = { 1.0f,1.0f,1.0f,0.95f };

	// ビュープロジェクションの初期化
	view = new ViewProjection;
	view->DebugCameraInitialze(input);
}

void GameScene::Update()
{
	// 入力の更新
	input->Update();

	// 数字の0キーが押されていたら
	if (input->PushKey(DIK_0))
	{
		OutputDebugStringA("Hit 0\n");  // 出力ウィンドウに「Hit 0」と表示
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
