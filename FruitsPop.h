#pragma once
#include <memory.h>
#include <list>
#include "Fruit.h"


// プレイヤークラスの前方宣言
class Player;

class FruitsPop
{
public:
	FruitsPop();
	~FruitsPop();

	// 初期化
	void Initialize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw(ViewProjection& view);

	// セッター
	void SetPlayer(Player* player);

private:
	// プレイヤー
	Player* player = nullptr;

	// フルーツの生成リスト
	std::list<std::unique_ptr<Fruit>> fruits;

	// フルーツのポップするタイマー
	int fruitPopTimer = 0;

	// フルーツのポップする間隔
	int fruitPopInterval = 0.5 * 60;

	// ポップする位置
	Vector3 popPos = { 0,3,1 };

	// ポップする種類
	int fruitsNum = 0;

	// フルーツのタイプ
	fruitType type = fruitType::apple;

	// モデル
	Model* model = nullptr;

	Model* appleModel;
	Model* orangeModel;
	Model* bananaModel;
	Model* bombModel;

	// スコアを記録するもの
	int score = 0;
};

