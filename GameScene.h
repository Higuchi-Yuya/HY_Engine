#pragma once
#include "Sound.h"
#include "Input.h"
#include "Sprite.h"
#include "Object3d.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Quaternion.h"
#include "Light.h"
#include "Player.h"
#include "FruitsPop.h"
#include "ScoreSprite.h"

class GameScene
{
public:
	enum class Scene {
		Title,
		Game,
		Result,

	};

	// �R���X�g���N�^
	GameScene();
	// �f�X�g���N�^
	~GameScene();

	// ����������
	void Initialize();

	// �X�V����
	void Update();

	// �`�揈��
	void Draw2Dback();

	void Draw3D();

	void Draw2Dfront();

private:// �v���C�x�[�g�֐�

	// �X�R�A�̕`�揈��
	void DrawScore();

	// �^�C���̕`�揈��
	void DrawTime();

	// �V�[�����Ƃ̃��Z�b�g
	void Reset();

	// �u���b�N�A�E�g
	void BlackOut();

private:// �����o�ϐ�

	// �V�[���Ǘ�
	Scene scene = Scene::Title;

	// �C���v�b�g
	Input* input = nullptr;

	// �I�u�W�F�N�g���ʂ̃��C�g
	Light* light = nullptr;

	// �e�N�X�`���n���h��
	int titleFontHandle = 0;
	int startFontHandle = 0;
	int spaceFontHandle = 0;

	int AkeyFontHandle = 0;
	int DkeyFontHandle = 0;
	int MoveFontHandle = 0;
	int TimeFontHandle = 0;
	int ScoreFontHandle = 0;

	int resultBoartHandle = 0;
	int ReplayFontHandle = 0;
	int titlemojiFontHandle = 0;


	int blackOutTex = 0;

	// �X�v���C�g
#pragma region �^�C�g���̃X�v���C�g
	Sprite* titleFont = nullptr;
	Sprite* startFont = nullptr;
	Sprite* spaceKey = nullptr;
#pragma endregion

#pragma region �Q�[���V�[���̃X�v���C�g
	Sprite* AkeyFont = nullptr;
	Sprite* DkeyFont = nullptr;
	Vector2 keyFontSize = { 48,48 };
	Sprite* MoveFont = nullptr;
	Vector2 moveFontSize = { 120,38 };
	Sprite* TimeFont = nullptr;
	Vector2 timeFontSize = { 73,25 };
	Sprite* ScoreFont = nullptr;
	Vector2 scoreFontSize = { 83,25 };
#pragma endregion

#pragma region ���U���g�V�[���̃X�v���C�g
	Sprite* resultBoart = nullptr;
	Sprite* replayFont = nullptr;
	Vector2 replayFontSize = { 184,64 };
	Sprite* titlemojiFont = nullptr;
	Vector2 titlemojiFontSize = { 184,64 };
#pragma endregion

#pragma region �T�E���h
	Sound titleBGM;
	Sound gameBGM;
	Sound resultBGM;
	bool titleBGMFlag = false;
	bool gameBGMFlag = false;
	bool resultBGMFlag = false;
#pragma endregion



	Sprite* blackOut = nullptr;


	// ���f��
	Model* skydomeModel = nullptr;
	Model* treeModel = nullptr;

	// �I�u�W�F�N�g
	Player* player = nullptr;
	Object3d* skydomeObj = nullptr;
	Object3d* treeObj = nullptr;

	// �r���[�v���W�F�N�V����
	ViewProjection* view = nullptr;

	// �t���[�c
	FruitsPop* fruitsPop = nullptr;

	// �X�R�A�p�ϐ�
	int divScore = 1000;
	const int maxScoreNum = 3;
	int scoreNum[3] = { 0 };
	Vector2 spriteBasePos = { 850,50 };
	Vector2 spritePos = { 1100,50 };
	ScoreSprite scoreSprite[3];

	// �^�C���p�ϐ�
	const int maxTimeNum = 2;
	int timeNum[2] = { 0 };
	Vector2 timeBasePos = { 180,50 };
	Vector2 timePos = { 1100,50 };
	ScoreSprite timeSprite[3];

	// �Q�[�����̃^�C�}�[
	int timer = 0;
	int timerInterval = 60;
	int gameTimer = 40;

	// �J�E���g�_�E��
	ScoreSprite countSprite;
	Vector2 countBasePos = { 520,280 };
	Vector2 countPos = { 1100,320 };

	bool countFlag = true;
	int countTimer = 0;
	int countTimeInterval = 60;
	int countTime = 3;


	// �u���b�N�A�E�g�̕ϐ�
	float blackAlpha = 0.0f;
	bool sceneChangeFlag = false;
	Scene oldScene = Scene::Title;
	bool resultChange = false;
};

