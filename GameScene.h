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

	// コンストラクタ
	GameScene();
	// デストラクタ
	~GameScene();

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw2Dback();

	void Draw3D();

	void Draw2Dfront();

private:// プライベート関数

	// スコアの描画処理
	void DrawScore();

	// タイムの描画処理
	void DrawTime();

	// シーンごとのリセット
	void Reset();

	// ブラックアウト
	void BlackOut();

private:// メンバ変数

	// シーン管理
	Scene scene = Scene::Title;

	// インプット
	Input* input = nullptr;

	// オブジェクト共通のライト
	Light* light = nullptr;

	// テクスチャハンドル
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

	// スプライト
#pragma region タイトルのスプライト
	Sprite* titleFont = nullptr;
	Sprite* startFont = nullptr;
	Sprite* spaceKey = nullptr;
#pragma endregion

#pragma region ゲームシーンのスプライト
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

#pragma region リザルトシーンのスプライト
	Sprite* resultBoart = nullptr;
	Sprite* replayFont = nullptr;
	Vector2 replayFontSize = { 184,64 };
	Sprite* titlemojiFont = nullptr;
	Vector2 titlemojiFontSize = { 184,64 };
#pragma endregion

#pragma region サウンド
	Sound titleBGM;
	Sound gameBGM;
	Sound resultBGM;
	bool titleBGMFlag = false;
	bool gameBGMFlag = false;
	bool resultBGMFlag = false;
#pragma endregion



	Sprite* blackOut = nullptr;


	// モデル
	Model* skydomeModel = nullptr;
	Model* treeModel = nullptr;

	// オブジェクト
	Player* player = nullptr;
	Object3d* skydomeObj = nullptr;
	Object3d* treeObj = nullptr;

	// ビュープロジェクション
	ViewProjection* view = nullptr;

	// フルーツ
	FruitsPop* fruitsPop = nullptr;

	// スコア用変数
	int divScore = 1000;
	const int maxScoreNum = 3;
	int scoreNum[3] = { 0 };
	Vector2 spriteBasePos = { 850,50 };
	Vector2 spritePos = { 1100,50 };
	ScoreSprite scoreSprite[3];

	// タイム用変数
	const int maxTimeNum = 2;
	int timeNum[2] = { 0 };
	Vector2 timeBasePos = { 180,50 };
	Vector2 timePos = { 1100,50 };
	ScoreSprite timeSprite[3];

	// ゲーム内のタイマー
	int timer = 0;
	int timerInterval = 60;
	int gameTimer = 40;

	// カウントダウン
	ScoreSprite countSprite;
	Vector2 countBasePos = { 520,280 };
	Vector2 countPos = { 1100,320 };

	bool countFlag = true;
	int countTimer = 0;
	int countTimeInterval = 60;
	int countTime = 3;


	// ブラックアウトの変数
	float blackAlpha = 0.0f;
	bool sceneChangeFlag = false;
	Scene oldScene = Scene::Title;
	bool resultChange = false;
};

