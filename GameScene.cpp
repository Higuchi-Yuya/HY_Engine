#include "GameScene.h"
#include <time.h>

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
	// �I�u�W�F�N�g�̉��
	delete skydomeObj;
	delete treeObj;
	// ���f���̉��
	delete skydomeModel;
	delete treeModel;
	// �r���[�̉��
	delete view;
	// �v���C���[�̉��
	delete player;
	// �t���[�c�S�̂̉��
	delete fruitsPop;
	//fruitsPop->Finalize();
}

void GameScene::Initialize()
{
	// �����_���p
	srand(time(nullptr));

	// ���͂̏�����
	input = new Input();
	input->Initialize();

	// ���C�g�̐���
	light = Light::Create();
	// ���C�g�̐F��ݒ�
	light->SetLightDir({ 0,-1,5 });
	// 3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g
	Object3d::SetLight(light);

	// �e�N�X�`���̏�����
	// �^�C�g���Ŏg���n���h��
	titleFontHandle = Texture::LoadTexture("Title-export.png");
	startFontHandle = Texture::LoadTexture("Start.png");
	spaceFontHandle = Texture::LoadTexture("Space.png");
	// �Q�[���V�[���Ŏg���n���h��
	AkeyFontHandle = Texture::LoadTexture("A_key.png");
	DkeyFontHandle = Texture::LoadTexture("D_key.png");
	MoveFontHandle = Texture::LoadTexture("Move.png");
	TimeFontHandle = Texture::LoadTexture("time.png");
	ScoreFontHandle = Texture::LoadTexture("Score.png");

	// ���U���g�V�[���Ŏg���n���h��
	resultBoartHandle = Texture::LoadTexture("scoreBoart.png");
	ReplayFontHandle = Texture::LoadTexture("replay.png");
	titlemojiFontHandle = Texture::LoadTexture("titleFont.png");


	// �V�[���`�F���W�Ŏg���n���h��
	blackOutTex = Texture::LoadTexture("kakusi.png");

	// �X�v���C�g�̏�����

	// �^�C�g���̃X�v���C�g��new
	titleFont = new Sprite();
	startFont = new Sprite();
	spaceKey = new Sprite();

	// �Q�[���V�[���̃X�v���C�g��new
	AkeyFont = new Sprite();
	DkeyFont = new Sprite();
	MoveFont = new Sprite();
	TimeFont = new Sprite();
	ScoreFont = new Sprite();

	// ���U���g�V�[���̃X�v���C�g��new
	resultBoart = new Sprite();
	replayFont = new Sprite();
	titlemojiFont = new Sprite();

	blackOut = new Sprite();


	// �X�v���C�g�̏����ݒ�
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
	// �r���[�v���W�F�N�V�����̏�����
	view = new ViewProjection;
	view->DebugCameraInitialze(input);

	// �v���C���[�̏�����
	player = new Player();
	player->Initialize();

	// �t���[�c�̏�����
	fruitsPop = new FruitsPop();
	fruitsPop->Initialize();

	// �X�R�A�X�v���C�g�̏�����
	for (int i = 0; i < maxScoreNum; i++)
	{
		scoreSprite[i].Initialize();
	}
	for (int i = 0; i < maxTimeNum; i++) {
		timeSprite[i].Initialize();
	}

	countSprite.Initialize();
	countSprite.SetSize({ 60,80 });

	// �X�J�C�h�[���̏�����
	skydomeModel = Model::LoadFromOBJ("aozora", true);

	skydomeObj = Object3d::Create();
	skydomeObj->SetModel(skydomeModel);

	skydomeObj->worldTransform_.scale_ = { 15.0f,15.0f,15.0f };
	skydomeObj->worldTransform_.color_ = { 0.4f,0.4f, 0.4f, 1.0f };
	skydomeObj->worldTransform_.rotation_ = { 0.0f,0.22f,0.0f };

	// �؂̏�����
	treeModel = Model::LoadFromOBJ("tree", true);

	treeObj = Object3d::Create();
	treeObj->SetModel(treeModel);

	treeObj->worldTransform_.position_ = { 0.0f,-4,5 };
	treeObj->worldTransform_.scale_ = { 10,10,10 };

	// ���̏�����
	titleBGM.SoundLoadWave("Resources/Sound/TitleBGM.wav");
	gameBGM.SoundLoadWave("Resources/Sound/GameBGM.wav");
	resultBGM.SoundLoadWave("Resources/Sound/resultBGM.wav");

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

	// �ǂ̃V�[���ł��g�����̂̍X�V����
	light->Update();
	view->UpdateMatrix();
	skydomeObj->Update();
	treeObj->Update();

	BlackOut();
	//view->DebugCameraUpdate();
	switch (scene)
	{
	case GameScene::Scene::Title: // �^�C�g���V�[��
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
	case GameScene::Scene::Game:  // �Q�[���V�[��
		if (gameBGMFlag == false) {
			gameBGM.SoundPlayWave(true, 0.5f);
			gameBGMFlag = true;
		}


		if (input->TriggerKey(DIK_N)) {
			gameTimer = 2;
		}

		player->Update();

		// �J�E���g�_�E���^�C�}�[
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
		// �J�E���g�I����̏���
		if (countFlag == false) {
			// �c�莞�Ԃ̏���
			timer++;
			if (timer >= timerInterval) {
				timer = 0;
				gameTimer -= 1;

			}
			fruitsPop->SetPlayer(player);
			fruitsPop->Update();
		}
		
		// �������ԂɂȂ�����
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
	case GameScene::Scene::Result:// ���U���g�V�[��
		if (resultBGMFlag == false) {
			resultBGM.SoundPlayWave(true, 0.5f);
			resultBGMFlag = true;
		}

		if (sceneChangeFlag == false) {
			// ���v���C�Ɉڍs
			if (input->TriggerKey(DIK_A)) {
				oldScene = Scene::Result;
				resultChange = false;
				spaceKey->SetPosition({ WinApp::window_width / 2 - 200,500 });
			}

			// �^�C�g���Ɉڍs
			if (input->TriggerKey(DIK_D)) {
				oldScene = Scene::Result;
				resultChange = true;
				spaceKey->SetPosition({ WinApp::window_width / 2 + 200,500 });
			}

			// ����{�^���������ꂽ��
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
	case GameScene::Scene::Title: // �^�C�g���V�[��

		break;
	case GameScene::Scene::Game:  // �Q�[���V�[��

		break;
	case GameScene::Scene::Result:// ���U���g�V�[��

		break;
	default:
		break;
	}

}

void GameScene::Draw3D()
{
	switch (scene)
	{
	case GameScene::Scene::Title: // �^�C�g���V�[��

		break;
	case GameScene::Scene::Game:  // �Q�[���V�[��
		player->Draw(view);
		fruitsPop->Draw(view);
		break;
	case GameScene::Scene::Result:// ���U���g�V�[��

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
	case GameScene::Scene::Title: // �^�C�g���V�[��
		titleFont->Draw();
		startFont->Draw();
		spaceKey->Draw();
		break;
	case GameScene::Scene::Game:  // �Q�[���V�[��
		//sprite2->Draw();
		AkeyFont->Draw();
		DkeyFont->Draw();
		MoveFont->Draw();
		TimeFont->Draw();
		ScoreFont->Draw();

		DrawTime();
		DrawScore();

		break;
	case GameScene::Scene::Result:// ���U���g�V�[��
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
	// �X�R�A�̕`��
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
	// �^�C���̕`��
	divScore = 100;
	for (int i = 0; i < maxTimeNum; i++)
	{
		timeNum[i] = gameTimer % divScore / (divScore / 10);
		divScore = divScore / 10;
		timePos.x = timeBasePos.x - ((1 - i) * timeSprite->GetSize().x);
		timeSprite[i].Draw(timeNum[i], timePos);
	}

	// �J�E���g�_�E���̕`��

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
		// �Q�[���̐�������
		timer = 0;
		gameTimer = 40;
		// �J�E���g�_�E���̎���
		countFlag = true;
		countTimer = 0;
		countTime = 3;
		// �v���C���[�̏�����
		player->Initialize();
		// �t���[�c�̏�����
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

		// �X�v���C�g�̐ݒ�
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
			// �Q�[���V�[�����烊�U���g�V�[��
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
