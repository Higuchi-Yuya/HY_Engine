#include "GameScene.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "Player.h"
#include "MeshCollider.h"
#include "TouchableObject.h"
#include <time.h>
#include "InputManager.h"
#include "FbxLoader.h"
#include "Random.h"
#include "BeatEffect.h"

GameScene::~GameScene()
{
	for (auto e : enemys_) {
		delete e;
	}
	enemys_.clear();
}

void GameScene::Initialize()
{
#pragma region モデル読み込み
	// プレイヤーのモデル読み込み
	playerModel_.reset(Model::LoadFromOBJ("CharacterYomawari", true));
#pragma endregion

#pragma region サウンド読み込み
	sound.SoundLoadWave("GameClear.wav");
#pragma endregion

#pragma region プレイヤー関連の初期化
	player_ = std::make_unique<Player>();
	player_.reset(Player::Create(playerModel_.get()));
	player_->InitializeFlashLightRange();

	gameCamera_->SetCameraFPos(player_->worldTransform_.translation);

	player_->SetGameCamera(gameCamera_);
#pragma endregion

#pragma region UI関連の初期化
	timerUi_ = std::make_unique<TimerUI>();
	timerUi_->Init();

	operationUi_ = std::make_unique<OperationUI>();
	operationUi_->Init();

#pragma endregion

#pragma region ローダー読み込み
	// レベルデータの読み込み
	levelData_.reset(LevelLoader::LoadFile("field"));

	for (auto& objectData : levelData_->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models_)::iterator it = models_.find(objectData.fileName);
		if (it != models_.end()) {
			model = it->second;
		}

		// タグ名がエネミーなら
#pragma region エネミー関連の初期化
		if (objectData.tagName == "enemy")
		{
			WorldTransform w;
			w.Initialize();
			w.translation = objectData.translation;
			w.scale = objectData.scaling;
			w.rotation = objectData.rotation;

			// 新しい敵の生成
			Enemy* newEnemy = new Enemy;
			newEnemy->SetWorldTransInfo(w);
			newEnemy->Initialize();
			newEnemy->UpdateWorldMatrix();

			// 今作成した敵を配列に格納
			enemys_.push_back(newEnemy);
			// 今作成した敵を当たり判定に情報を登録
			gameCollider_->AddEnemy(newEnemy);
		}
#pragma endregion
	}
#pragma endregion

#pragma region コライダー関連の初期化
	for (auto e : enemys_) {
		gameCollider_->AddEnemy(e);
	}

	gameCollider_->SetPlayer(player_.get());
#pragma endregion

	Random::Init();
	// 入力の初期化
	input_ = std::make_unique<Input>();
	input_->Initialize();

	Enemy::SetPlayer(player_.get());
	BeatEffect::SetPlayer(player_.get());
	BeatEffect::SetEnemys(enemys_);

}

void GameScene::Update()
{
	input_->Update();

	GameSceneUpdate();
}

void GameScene::ImguiUpdate()
{
	// 表示項目の追加--------//
	// ---------------------//
	ImGui::SetNextWindowSize(ImVec2(500, 100));
	ImGui::Begin("Sound");

	ImGui::Checkbox("Is Active", &isActiveSound);
	ImGui::Checkbox("Is Stop", &isStopSound);

	ImGui::InputFloat("spotLightDownPosY", &spotDownPosY);
	ImGui::InputFloat("playerPointIndensity", &playerPointLightIndensity);
	ImGui::InputFloat("playerPointRadius", &playerPointLightRadius_);
	ImGui::InputFloat("playerPointDecay", &playerPointLightDecay_);

	ImGui::End();

	// 時間のImgui
	timerUi_->DrawImgui();

	// プレイヤーのimgui
	player_->DrawImgui();
}

void GameScene::Draw2DBack()
{
}

void GameScene::Draw3D()
{
	// オブジェクト関連の描画

	DrawShieldObj();


	// お墓のドアのオブジェクトの描画
	for (auto d : latticeDoors_) {
		d->Draw(&gameCamera_->GetView());
	}

	// 普通のオブジェクトの描画
	for (auto o : objects_) {
		o->Draw(&gameCamera_->GetView());
	}
	gameCollider_->Draw3D(&gameCamera_->GetView());
	// プレイヤーの描画
	player_->Draw(&gameCamera_->GetView());

	DrawTransParentObj();
	DrawParticle();
}

void GameScene::DrawParticle()
{
	ParticleManager::PreDraw(commandList_);
	gameCollider_->Draw(&gameCamera_->GetView());
	// 敵の描画
	for (auto e : enemys_) {
		e->DrawParticle(&gameCamera_->GetView());
	}
	ParticleManager::PostDraw();
}

void GameScene::Draw2DFront()
{
	player_->Draw2DFront();
	timerUi_->DrawFrontSprite();
	operationUi_->DrawFrontSprite();
}

void GameScene::DrawBloomObject()
{
	// ランタンのオブジェクトの描画
	for (auto L : ranterns_) {
		L->worldTransform_.IsBloom_ = 0;
		L->worldTransform_.UpdateMatrix();
		L->Draw(&gameCamera_->GetView());
	}
}

void GameScene::DrawHighLumiObj()
{
	// ランタンのオブジェクトの描画
	for (auto L : highLumiRanterns_) {
		L->worldTransform_.IsBloom_ = 1;
		L->worldTransform_.UpdateMatrix();
		L->Draw(&gameCamera_->GetView());
	}
}

void GameScene::DrawTransParentObj()
{
	// オブジェクトのパイプラインをステンシルの読み込み側に変更
	Object3d::SetBlendMode(Object3d::BlendMode::NORMAL);

	// 敵の描画
	for (auto e : enemys_) {
		e->Draw(&gameCamera_->GetView());
	}

	Object3d::SetBlendMode(Object3d::BlendMode::NORMAL);

	// ステンシルの参照値を0に戻す
	commandList_->OMSetStencilRef(0);
}

void GameScene::DrawShieldObj()
{
	// ステンシルテストの参照値を設定する
	commandList_->OMSetStencilRef(2);

	// オブジェクトのパイプラインをステンシルの書き込み側に変更
	Object3d::SetBlendMode(Object3d::BlendMode::Shield);

	// プレイヤーの懐中電灯の範囲オブジェクトの描画
	player_->DrawFlashLightRange(&gameCamera_->GetView());

	Object3d::SetBlendMode(Object3d::BlendMode::NORMAL);
}

void GameScene::Reset()
{
	IsSceneFinsh_ = false;
	// ゲームクリアのフラグリセット
	IsGameClear_ = false;

	// ウェーブのリセット
	enemyWave_ = EnemyWave::wave01;
	waveTimeNum_ = 0;
	waveTimer_ = 0;

	// ゲームコライダーのリセット
	gameCollider_->Reset();

	//	エネミーを全削除
	for (auto e : enemys_) {
		delete e;
	}
	enemys_.clear();

	// エネミーの再読み込み
	LoadEnemy();

	// エネミーのスポーンする間隔をリセット
	enemySpawnTimer_ = 0;

	// プレイヤーのリセット
	player_->Reset();

	// 時間のリセット
	timerUi_->Reset();

	// カメラのリセット
	gameCamera_->Reset();

}

void GameScene::LoadEnemy()
{
	switch (enemyWave_)
	{
	case GameScene::wave01:// ウェーブ０１の時の敵のスポーンパターン抽選

		randomWave01_ = Random::Range(1, 3);

		switch (randomWave01_)
		{
		case 1:
			// レベルデータの読み込み
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave01_p1"));
			break;
		case 2:
			// レベルデータの読み込み
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave01_p2"));
			break;
		case 3:
			// レベルデータの読み込み
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave01_p3"));
			break;
		default:
			break;
		}

		break;
	case GameScene::wave02:// ウェーブ０２の時の敵のスポーンパターン抽選
		randomWave01_ = Random::Range(1, 3);

		switch (randomWave01_)
		{
		case 1:
			// レベルデータの読み込み
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave02_p1"));
			break;
		case 2:
			// レベルデータの読み込み
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave02_p2"));
			break;
		case 3:
			// レベルデータの読み込み
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave02_p3"));
			break;
		default:
			break;
		}

		break;
	case GameScene::wave03:// ウェーブ０３の時の敵のスポーンパターン抽選

		randomWave01_ = Random::Range(1, 3);

		switch (randomWave01_)
		{
		case 1:
			// レベルデータの読み込み
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave03_p1"));
			break;
		case 2:
			// レベルデータの読み込み
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave03_p2"));
			break;
		case 3:
			// レベルデータの読み込み
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave03_p3"));
			break;
		default:
			break;
		}

		break;
	default:
		break;
	}


	for (auto& objectData : levelData_->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models_)::iterator it = models_.find(objectData.fileName);
		if (it != models_.end()) {
			model = it->second;
		}

		// タグ名がエネミーなら
#pragma region エネミー関連の初期化
		if (objectData.tagName == "enemy")
		{
			WorldTransform w;
			w.Initialize();
			w.translation = objectData.translation;
			w.scale = objectData.scaling;
			w.rotation = objectData.rotation;

			// 新しい敵の生成
			Enemy* newEnemy = new Enemy;
			newEnemy->SetWorldTransInfo(w);
			newEnemy->Initialize();
			newEnemy->UpdateWorldMatrix();

			// 今作成した敵を配列に格納
			enemys_.push_back(newEnemy);
			// 今作成した敵を当たり判定に情報を登録
			gameCollider_->AddEnemy(newEnemy);
		}
#pragma endregion
	}
}

void GameScene::SetObjs(std::vector<Object3d*> objs, ObjsType objType)
{
	switch (objType)
	{
	case ObjsType::Normal:
		objects_ = objs;
		break;
	case ObjsType::Doors:
		latticeDoors_ = objs;
		break;
	case ObjsType::Ranterns:
		ranterns_ = objs;
		break;
	case ObjsType::HiguLumiRanterns:
		highLumiRanterns_ = objs;
		break;
	default:
		break;
	}
}

void GameScene::GameSceneUpdate()
{
	// 入力の更新

	if (isActiveSound == true) {
		sound.SoundPlayWave(true, 0.01f);
		isActiveSound = false;
	}
	if (isStopSound == true) {
		sound.StopWave();
		isStopSound = false;
	}

	// プレイヤーの更新処理
	player_->Update();

	// プレイヤーが死んだらとりあえずリザルト画面に移行
	if (player_->GetIsAlive() == false) {
		IsSceneFinsh_ = true;
		IsGameClear_ = false;
	}

	// 敵の更新処理関連
	EnemyGameUpdate();

	// カメラの更新処理
	gameCamera_->SetCameraPos(player_->worldTransform_.translation);
	gameCamera_->GameUpdate();

	// お墓のドアのオブジェクトの描画
	for (auto l : latticeDoors_) {
		l->Update();
	}

	// オブジェクトの更新処理
	for (auto o : objects_) {
		o->Update();
	}

	// 当たり判定関連の更新処理
	gameCollider_->Updata();

	// タイマーの更新処理
	timerUi_->Update();

	// オペレーションの更新処理
	operationUi_->Update();

	// ライトの更新処理
	LightUpdate();

	// 制限時間が過ぎたらクリア
	if (timerUi_->GetIsTimeEnd() == true) {
		IsSceneFinsh_ = true;
		IsGameClear_ = true;
	}
}

void GameScene::LightUpdate()
{
	lightGroup_->SetSpotLightActive(0, true);
	lightGroup_->SetCircleShadowActive(0, true);
	lightGroup_->SetPointLightActive((int)(pointLightsInfo_.size() + 1), true);

	Vector3 spotPos = player_->GetWorldPosition();
	spotPos.y += 1;

	Vector3 playerFrontVec = player_->GetFrontVec();
	playerFrontVec.y -= spotDownPosY;
	playerFrontVec.normalize();

	lightGroup_->SetSpotLightDir(0, playerFrontVec);
	lightGroup_->SetSpotLightPos(0, spotPos);
	lightGroup_->SetPointLightPos((int)(pointLightsInfo_.size() + 1), spotPos);

	lightGroup_->SetPointLightIndensity((int)(pointLightsInfo_.size() + 1), playerPointLightIndensity);
	lightGroup_->SetPointLightRadius((int)(pointLightsInfo_.size() + 1), playerPointLightRadius_);
	lightGroup_->SetPointLightDecay((int)(pointLightsInfo_.size() + 1), playerPointLightDecay_);

	// ライトの更新処理
	circleShadowCasterPos = player_->GetWorldPosition();
	lightGroup_->SetCircleShadowCasterPos(0, circleShadowCasterPos);
	lightGroup_->Update();
}

void GameScene::EnemyGameUpdate()
{
	enemySpawnTimer_++;

	// エネミーのウェーブの時間を進める
	waveTimer_++;

	// エネミーのウェーブの時間がマックスになったらウェーブを進める
	if (waveTimer_ >= waveTimeMax_) {
		if (waveTimeNum_ < 3) {
			waveTimeNum_++;
		}
		switch (waveTimeNum_)
		{
		case 0:
			enemyWave_ = wave01;
			break;
		case 1:
			enemyWave_ = wave02;
			break;
		case 2:
			enemyWave_ = wave03;
			break;
		default:
			break;
		}

		// ウェーブの時間をリセット
		waveTimer_ = 0;
	}

	// エネミーの時間ごとにわく処理 (無限沸き)
	switch (enemyWave_)
	{
	case GameScene::wave01:
		if (enemySpawnTimer_ >= enemySpawnTimeMax1_ && enemys_.size() <= 3 * 1) {
			LoadEnemy();
			enemySpawnTimer_ = 0;
		}
		break;
	case GameScene::wave02:
		if (enemySpawnTimer_ >= enemySpawnTimeMax2_ && enemys_.size() <= 4 * 2) {
			LoadEnemy();
			enemySpawnTimer_ = 0;
		}
		break;
	case GameScene::wave03:
		if (enemySpawnTimer_ >= enemySpawnTimeMax3_ && enemys_.size() <= 5 * 3) {
			LoadEnemy();
			enemySpawnTimer_ = 0;
		}
		break;
	default:
		break;
	}

	//寿命が尽きた敵を全削除
	auto it = std::partition(enemys_.begin(), enemys_.end(), [](Enemy* a)
		{return a->GetDeadMotionEnd() == true; });
	std::for_each(enemys_.begin(), it, [](Enemy* a) { delete a; });
	enemys_.erase(enemys_.begin(), it);

	//	敵の更新処理
	for (auto e : enemys_) {
		e->Update();
	}

	BeatEffect::SetEnemys(enemys_);
}
