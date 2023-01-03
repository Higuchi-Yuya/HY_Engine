#include "FruitsPop.h"
#include "Player.h"

FruitsPop::FruitsPop()
{
	appleModel = Model::LoadFromOBJ("Medama", true);
	orangeModel = Model::LoadFromOBJ("herfSphere", true);
	bananaModel = Model::LoadFromOBJ("sphere", true);
	bombModel = Model::LoadFromOBJ("cube");

}

FruitsPop::~FruitsPop()
{

	//fruits.clear();

	delete model;
	delete appleModel;
	delete orangeModel;
	delete bananaModel;
	delete bombModel;
	delete player;
}

void FruitsPop::Initialize()
{

	fruits.clear();

	fruitPopTimer = 0;
	fruitPopInterval = 0.5 * 60;
	score = 0;
}

void FruitsPop::Update()
{
	fruitPopTimer++;

	// フルーツの消滅フラグ立っていたらリストから消す
	//fruits.remove_if([](std::unique_ptr<Fruit>& fruit) { return fruit->IsDeath(); });
	//for (auto itr = fruits.begin(); itr != fruits.end();) {
	//	if (itr->get()->IsDeath() == true)
	//	{
	//		itr = fruits.erase(itr);
	//	}
	//	else {
	//		++itr;
	//	}
	//}

	// フルーツのポップタイムになったらポップ
	if (fruitPopTimer >= fruitPopInterval) {
		// ランダムにポジションを抽選
		// 範囲は（-10〜10）
		popPos.x = rand() % 21 - 10;

		// ランダムに種類を抽選 (0〜3)
		fruitsNum = rand() % 4;

		// フルーツの番号でタイプの指定
		switch (fruitsNum)
		{
		case 0:// りんご
			type = fruitType::apple;
			break;

		case 1:// オレンジ
			type = fruitType::orange;
			break;

		case 2:// バナナ
			type = fruitType::banana;
			break;

		case 3:// 爆弾
			type = fruitType::bomb;
			break;

		default:
			break;
		}

		// フルーツのタイプでモデルの指定
		switch (type)
		{
		case fruitType::apple:
		{
			// フルーツの生成、初期化
			std::unique_ptr<Fruit> newFruit = std::make_unique<Fruit>();

			newFruit->Initialize(popPos, appleModel);
			newFruit->typeNum = 0;
			// リストに登録
			fruits.push_back(std::move(newFruit));
		}
		break;
		case fruitType::orange:
		{
			// フルーツの生成、初期化
			std::unique_ptr<Fruit> newFruit = std::make_unique<Fruit>();

			newFruit->Initialize(popPos, orangeModel);
			newFruit->typeNum = 1;
			// リストに登録
			fruits.push_back(std::move(newFruit));
		}
		break;
		case fruitType::banana:
		{
			// フルーツの生成、初期化
			std::unique_ptr<Fruit> newFruit = std::make_unique<Fruit>();

			newFruit->Initialize(popPos, bananaModel);
			newFruit->typeNum = 2;
			// リストに登録
			fruits.push_back(std::move(newFruit));
		}
		break;
		case fruitType::bomb:
		{
			// フルーツの生成、初期化
			std::unique_ptr<Fruit> newFruit = std::make_unique<Fruit>();

			newFruit->Initialize(popPos, bombModel);
			newFruit->typeNum = 3;
			// リストに登録
			fruits.push_back(std::move(newFruit));
		}
		break;
		default:
			break;
		}

		// ポップタイマーのリセット
		fruitPopTimer = 0;
	}

	// フルーツのリストの更新処理呼び出し
	for (std::unique_ptr<Fruit>& fruit : fruits) {
		fruit->Update(player);

		if (fruit->IsHit() == true) {
			switch (fruit->typeNum)
			{
			case 0:// りんごの点数
				score += 5;
				break;
			case 1:// オレンジの点数
				score += 5;
				break;
			case 2:// バナナの点数
				score += 10;
				break;
			case 3:// 爆弾の点数
				score -= 20;
			default:
				break;
			}
		}
	}

	// スコアが０以下になったら０にする
	if (score <= 0) {
		score = 0;
	}
}

void FruitsPop::Draw(ViewProjection& view)
{
	// フルーツの消滅フラグ立っていたらリストから消す
	//fruits.remove_if([](std::unique_ptr<Fruit>& fruit) { return fruit->IsDeath(); });

	for (std::unique_ptr<Fruit>& fruit : fruits) {
		fruit->Draw(&view);
	}


}

void FruitsPop::SetPlayer(Player* player)
{
	this->player = player;
}
