#include "GameScene.h"
#include <time.h>

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
	delete titleFont;
	delete startFont;
	delete spaceKey;

	delete AkeyFont;
	delete DkeyFont;
	delete MoveFont;
	delete TimeFont;
	delete ScoreFont;

	delete resultBoart;
	delete replayFont;
	delete titlemojiFont;

	delete blackOut;
	// オブジェクトの解放
	delete skydomeObj;
	delete treeObj;
	// モデルの解放
	delete skydomeModel;
	delete treeModel;
	// ビューの解放
	delete view;
	// プレイヤーの解放
	delete player;
	// フルーツ全体の解放
	delete fruitsPop;
	//fruitsPop->Finalize();
}

void GameScene::Initialize()
{
	// ランダム用
	srand(time(nullptr));

	// 入力の初期化
	input = new Input();
	input->Initialize();

	// ライトの生成
	light = Light::Create();
	// ライトの色を設定
	light->SetLightDir({ 0,-1,5 });
	// 3Dオブジェクトにライトをセット
	Object3d::SetLight(light);

	// テクスチャの初期化
	// タイトルで使うハンドル
	titleFontHandle = Texture::LoadTexture("Title-export.png");
	startFontHandle = Texture::LoadTexture("Start.png");
	spaceFontHandle = Texture::LoadTexture("Space.png");
	// ゲームシーンで使うハンドル
	AkeyFontHandle = Texture::LoadTexture("A_key.png");
	DkeyFontHandle = Texture::LoadTexture("D_key.png");
	MoveFontHandle = Texture::LoadTexture("Move.png");
	TimeFontHandle = Texture::LoadTexture("time.png");
	ScoreFontHandle = Texture::LoadTexture("Score.png");

	// リザルトシーンで使うハンドル
	resultBoartHandle = Texture::LoadTexture("scoreBoart.png");
	ReplayFontHandle = Texture::LoadTexture("replay.png");
	titlemojiFontHandle = Texture::LoadTexture("titleFont.png");


	// シーンチェンジで使うハンドル
	blackOutTex = Texture::LoadTexture("kakusi.png");

	// スプライトの初期化

	// タイトルのスプライトのnew
	titleFont = new Sprite();
	startFont = new Sprite();
	spaceKey = new Sprite();

	// ゲームシーンのスプライトのnew
	AkeyFont = new Sprite();
	DkeyFont = new Sprite();
	MoveFont = new Sprite();
	TimeFont = new Sprite();
	ScoreFont = new Sprite();

	// リザルトシーンのスプライトのnew
	resultBoart = new Sprite();
	replayFont = new Sprite();
	titlemojiFont = new Sprite();

	blackOut = new Sprite();


	// スプライトの初期設定
	titleFont->Initialize(titleFontHandle, { WinApp::window_width / 2,200 });
	titleFont->SetSize({ 480,360 });
	startFont->Initialize(startFontHandle, { WinApp::window_width / 2,462 });
	startFont->SetSize({ 140,51 });
	spaceKey->Initialize(spaceFontHandle, { WinApp::window_width / 2,520 });
	spaceKey->SetSize({ 140,40 });

	AkeyFont->Initialize(AkeyFontHandle, { 60,400 });
	AkeyFont->SetSize(keyFontSize);
	DkeyFont->Initialize(DkeyFontHandle, { 110,400 });
	DkeyFont->SetSize(keyFontSize);
	MoveFont->Initialize(MoveFontHandle, { 90,450 });
	MoveFont->SetSize(moveFontSize);
	TimeFont->Initialize(TimeFontHandle, { 70,40 });
	TimeFont->SetSize(timeFontSize);
	ScoreFont->Initialize(ScoreFontHandle, { 740,40 });
	ScoreFont->SetSize(scoreFontSize);

	resultBoart->Initialize(resultBoartHandle, { WinApp::window_width / 2,WinApp::window_height / 2 });
	replayFont->Initialize(ReplayFontHandle, { WinApp::window_width / 2 - 200,450 });
	replayFont->SetSize(replayFontSize);
	titlemojiFont->Initialize(titlemojiFontHandle, { WinApp::window_width / 2 + 200,450 });
	titlemojiFont->SetSize(titlemojiFontSize);


	blackOut->Initialize(blackOutTex, { WinApp::window_width / 2,WinApp::window_height / 2 },{1280*2,720*2});
	blackOut->SetColor({ 1,1,1,blackAlpha });
	// ビュープロジェクションの初期化
	view = new ViewProjection;
	view->DebugCameraInitialze(input);

	// プレイヤーの初期化
	player = new Player();
	player->Initialize();

	// フルーツの初期化
	fruitsPop = new FruitsPop();
	fruitsPop->Initialize();

	// スコアスプライトの初期化
	for (int i = 0; i < maxScoreNum; i++)
	{
		scoreSprite[i].Initialize();
	}
	for (int i = 0; i < maxTimeNum; i++) {
		timeSprite[i].Initialize();
	}

	countSprite.Initialize();
	countSprite.SetSize({ 60,80 });

	// スカイドームの初期化
	skydomeModel = Model::LoadFromOBJ("aozora", true);

	skydomeObj = Object3d::Create();
	skydomeObj->SetModel(skydomeModel);

	skydomeObj->worldTransform_.scale_ = { 15.0f,15.0f,15.0f };
	skydomeObj->worldTransform_.color_ = { 0.4f,0.4f, 0.4f, 1.0f };
	skydomeObj->worldTransform_.rotation_ = { 0.0f,0.22f,0.0f };

	// 木の初期化
	treeModel = Model::LoadFromOBJ("tree", true);

	treeObj = Object3d::Create();
	treeObj->SetModel(treeModel);

	treeObj->worldTransform_.position_ = { 0.0f,-4,5 };
	treeObj->worldTransform_.scale_ = { 10,10,10 };

	// 音の初期化
	titleBGM.SoundLoadWave("Resources/Sound/TitleBGM.wav");
	gameBGM.SoundLoadWave("Resources/Sound/GameBGM.wav");
	resultBGM.SoundLoadWave("Resources/Sound/resultBGM.wav");

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

	// どのシーンでも使うものの更新処理
	light->Update();
	view->UpdateMatrix();
	skydomeObj->Update();
	treeObj->Update();

	BlackOut();
	//view->DebugCameraUpdate();
	switch (scene)
	{
	case GameScene::Scene::Title: // タイトルシーン
		if (titleBGMFlag == false) {
			titleBGM.SoundPlayWave(true, 0.5f);
			titleBGMFlag = true;
		}
		if (input->TriggerKey(DIK_SPACE))
		{
			titleBGM.StopWave();
			oldScene = Scene::Title;
			sceneChangeFlag = true;
		}

		break;
	case GameScene::Scene::Game:  // ゲームシーン
		if (gameBGMFlag == false) {
			gameBGM.SoundPlayWave(true, 0.5f);
			gameBGMFlag = true;
		}


		if (input->TriggerKey(DIK_N)) {
			gameTimer = 2;
		}

		player->Update();

		// カウントダウンタイマー
		if (countFlag == true) {
			countTimer++;
			if (countTimer >= countTimeInterval) {
				countTimer = 0;
				countTime -= 1;
			}
			if (countTime <= 0) {
				countTime = 0;
				countFlag = false;
			}
		}
		// カウント終了後の処理
		if (countFlag == false) {
			// 残り時間の処理
			timer++;
			if (timer >= timerInterval) {
				timer = 0;
				gameTimer -= 1;

			}
			fruitsPop->SetPlayer(player);
			fruitsPop->Update();
		}
		
		// 制限時間になったら
		if (gameTimer <= 0) {
			gameBGM.StopWave();
			gameTimer = 0;
			oldScene = Scene::Game;
			sceneChangeFlag = true;
		}

		if (input->TriggerKey(DIK_Z)) {
			fruitsPop->Initialize();
		}
		break;
	case GameScene::Scene::Result:// リザルトシーン
		if (resultBGMFlag == false) {
			resultBGM.SoundPlayWave(true, 0.5f);
			resultBGMFlag = true;
		}

		if (sceneChangeFlag == false) {
			// リプレイに移行
			if (input->TriggerKey(DIK_A)) {
				oldScene = Scene::Result;
				resultChange = false;
				spaceKey->SetPosition({ WinApp::window_width / 2 - 200,500 });
			}

			// タイトルに移行
			if (input->TriggerKey(DIK_D)) {
				oldScene = Scene::Result;
				resultChange = true;
				spaceKey->SetPosition({ WinApp::window_width / 2 + 200,500 });
			}

			// 決定ボタンを押されたら
			if (input->TriggerKey(DIK_SPACE)) {
				if (resultChange == false) {
					resultBGM.StopWave();
					sceneChangeFlag = true;
				}
				else if (resultChange == true) {
					resultBGM.StopWave();
					sceneChangeFlag = true;
				}
			}
		}


		break;
	default:
		break;
	}
}

void GameScene::Draw2Dback()
{
	//sprite->SetColor({ 1,1,1,0.5f });
	//sprite->Draw();

	switch (scene)
	{
	case GameScene::Scene::Title: // タイトルシーン

		break;
	case GameScene::Scene::Game:  // ゲームシーン

		break;
	case GameScene::Scene::Result:// リザルトシーン

		break;
	default:
		break;
	}

}

void GameScene::Draw3D()
{
	switch (scene)
	{
	case GameScene::Scene::Title: // タイトルシーン

		break;
	case GameScene::Scene::Game:  // ゲームシーン
		player->Draw(view);
		fruitsPop->Draw(view);
		break;
	case GameScene::Scene::Result:// リザルトシーン

		break;
	default:
		break;
	}
	treeObj->Draw(view);
	skydomeObj->Draw(view);
}

void GameScene::Draw2Dfront()
{
	switch (scene)
	{
	case GameScene::Scene::Title: // タイトルシーン
		titleFont->Draw();
		startFont->Draw();
		spaceKey->Draw();
		break;
	case GameScene::Scene::Game:  // ゲームシーン
		//sprite2->Draw();
		AkeyFont->Draw();
		DkeyFont->Draw();
		MoveFont->Draw();
		TimeFont->Draw();
		ScoreFont->Draw();

		DrawTime();
		DrawScore();

		break;
	case GameScene::Scene::Result:// リザルトシーン
		resultBoart->Draw();

		replayFont->Draw();
		titlemojiFont->Draw();

		AkeyFont->Draw();
		DkeyFont->Draw();
		spaceKey->Draw();

		DrawScore();

		ScoreFont->Draw();

		break;
	default:
		break;
	}
	blackOut->Draw();
}

void GameScene::DrawScore()
{
	// スコアの描画
	divScore = 1000;
	for (int i = 0; i < maxScoreNum; i++)
	{
		scoreNum[i] = fruitsPop->GetScore() % divScore / (divScore / 10);
		divScore = divScore / 10;
		spritePos.x = spriteBasePos.x - ((1 - i) * scoreSprite->GetSize().x);
		scoreSprite[i].Draw(scoreNum[i], spritePos);
	}
}

void GameScene::DrawTime()
{
	// タイムの描画
	divScore = 100;
	for (int i = 0; i < maxTimeNum; i++)
	{
		timeNum[i] = gameTimer % divScore / (divScore / 10);
		divScore = divScore / 10;
		timePos.x = timeBasePos.x - ((1 - i) * timeSprite->GetSize().x);
		timeSprite[i].Draw(timeNum[i], timePos);
	}

	// カウントダウンの描画

	if (countFlag == true) {
		divScore = 10;
		for (int i = 0; i < 1; i++)
		{
			int countNum = 0;
			countNum = countTime % divScore / (divScore / 10);
			divScore = divScore / 10;
			countPos.x = countBasePos.x - ((1 - i) * countSprite.GetSize().x);
			if (countNum != 0) {
				countSprite.Draw(countNum, countPos);
			}
		}
	}
}

void GameScene::Reset()
{
	switch (scene)
	{
	case GameScene::Scene::Title:
		titleBGMFlag = false;
		spaceKey->SetPosition({ WinApp::window_width / 2,520 });
		break;
	case GameScene::Scene::Game:
		gameBGMFlag = false;
		// ゲームの制限時間
		timer = 0;
		gameTimer = 40;
		// カウントダウンの時間
		countFlag = true;
		countTimer = 0;
		countTime = 3;
		// プレイヤーの初期化
		player->Initialize();
		// フルーツの初期化
		fruitsPop->Initialize();

		ScoreFont->SetSize({ 83,25 });
		ScoreFont->SetPosition({ 740,40 });
		spriteBasePos = { 850,50 };
		spritePos = { 1100,50 };

		AkeyFont->SetPosition({ 60,400 });
		DkeyFont->SetPosition({ 110,400 });

		break;
	case GameScene::Scene::Result:
		resultBGMFlag = false;
		resultChange = false;

		// スプライトの設定
		ScoreFont->SetSize({ 208, 64 });
		ScoreFont->SetPosition({ WinApp::window_width / 2,WinApp::window_height / 2 - 150 });
		spriteBasePos = { WinApp::window_width / 2 ,WinApp::window_height / 2 };
		spritePos.y = WinApp::window_height / 2 - 50;

		AkeyFont->SetPosition({ WinApp::window_width / 2 - 200,390 });
		DkeyFont->SetPosition({ WinApp::window_width / 2 + 200,390 });
		spaceKey->SetPosition({ WinApp::window_width / 2 - 200,500 });
		break;
	default:
		break;
	}
}

void GameScene::BlackOut()
{
	if (sceneChangeFlag == true) {
		switch (scene)
		{
		case GameScene::Scene::Title:
			if (oldScene == Scene::Result) {
				blackAlpha -= 0.025f;
				blackOut->SetColor({ 1,1,1,blackAlpha });
				if (blackAlpha <= 0) {
					blackAlpha = 0;
					sceneChangeFlag = false;
				}
			}
			else{
				blackAlpha += 0.025f;
				blackOut->SetColor({ 1,1,1,blackAlpha });
				if (blackAlpha >= 1) {
					blackAlpha = 1;
					scene = Scene::Game;
					Reset();
				}
			}

			break;
		case GameScene::Scene::Game:
			if (oldScene == Scene::Title || oldScene == Scene::Result) {
				blackAlpha -= 0.025f;
				blackOut->SetColor({ 1,1,1,blackAlpha });
				if (blackAlpha <= 0) {
					blackAlpha = 0;
					sceneChangeFlag = false;
				}
			}
			// ゲームシーンからリザルトシーン
			else {
				blackAlpha += 0.025f;
				blackOut->SetColor({ 1,1,1,blackAlpha });
				if (blackAlpha >= 1) {
					blackAlpha = 1;
					scene = Scene::Result;
					Reset();
				}
			}
			break;
		case GameScene::Scene::Result:
			if (oldScene == Scene::Game) {
				blackAlpha -= 0.025f;
				blackOut->SetColor({ 1,1,1,blackAlpha });
				if (blackAlpha <= 0) {
					blackAlpha = 0;
					sceneChangeFlag = false;
				}
			}
			else {
				blackAlpha += 0.025f;
				blackOut->SetColor({ 1,1,1,blackAlpha });
				if (blackAlpha >= 1) {
					blackAlpha = 1;
				
					if (resultChange == false) {
						scene = Scene::Game;
						Reset();
					}
					else if (resultChange == true) {
						scene = Scene::Title;
						Reset();
					}
				}
			}
			break;
		default:
			break;
		}
	}

}
