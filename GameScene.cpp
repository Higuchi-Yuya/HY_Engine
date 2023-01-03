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
	delete obj_2;
	// モデルの解放
	delete model_2;
	// ビューの解放
	delete view;

	//delete fruitsPop;

	// プレイヤーの解放
	delete player;


	
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

	// ビュープロジェクションの初期化
	view = new ViewProjection;
	view->DebugCameraInitialze(input);

	// プレイヤーの初期化
	player = new Player();
	player->Initialize();

	// フルーツの初期化
	fruitsPop = new FruitsPop();
	fruitsPop->Initialize();


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
