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


GameScene::~GameScene()
{
	models.clear();
	for (auto e : enemys_) {
		delete e;
	}
	enemys_.clear();
	for (auto o : objects) {
		delete o;
	}
	objects.clear();

	for (auto o : latticeDoors_) {
		delete o;
	}
	latticeDoors_.clear();

	Enemy::StaticFinalize();
}

void GameScene::Initialize()
{
	Random::Init();
	// 入力の初期化
	input_ = std::make_unique<Input>();
	input_->Initialize();
	Enemy::StaticInitialize();
#pragma region ライトの初期化
	// ライトの生成
	light.reset(LightGroup::Create());
	// ライト設定
	light->SetDirLightActive(0, true);
	light->SetDirLightActive(1, false);
	light->SetDirLightActive(2, false);
	//light->SetPointLightActive(0, true);
	//pointLightPos[0] = 0.5f;
	//pointLightPos[1] = 1.0f;
	//pointLightPos[2] = 0.0f;

	light->SetPointLightActive(0, false);
	light->SetPointLightActive(1, false);
	light->SetPointLightActive(2, false);

	light->SetSpotLightActive(0, false);

	light->SetCircleShadowActive(0, true);

	// 3Dオブジェクトにライトをセット
	Object3d::SetLight(light.get());
#pragma endregion

#pragma region フォグ
	fog.reset(Fog::Create());
	fog->nearFog = 10;
	fog->farFog = 100;
	fog->isActiveFog = false;
	Object3d::SetFog(fog.get());
#pragma endregion

#pragma region テクスチャ読み込み
	// テクスチャハンドルの読み込み

	// 仮のスプライトのハンドル
	textureHandleDefu.reset(TextureManager::Load2DTextureP("texture.png"));
	
	// シーンチェンジで使うハンドル
	blackOutTexHandle_.reset(TextureManager::Load2DTextureP("SceneChageTex.png"));

	// タイトルの文字で使うハンドル（仮）
	titleFontTexHandle.reset(TextureManager::Load2DTextureP("kariTitle.png"));

	// タイトルの背景で使うハンドル（仮）
	titleBackTexHandle.reset(TextureManager::Load2DTextureP("titleBack.png"));

	// タイトルのAボタン押すフォント（仮）
	titleButtonTexHandle.reset(TextureManager::Load2DTextureP("PressA.png"));


#pragma endregion

#pragma region モデルの読み込み
	// プレイヤーのモデル読み込み
	playerModel_.reset(Model::LoadFromOBJ("chr_sword", true));

	// 敵のモデル読み込み
	modelMedama_.reset(Model::LoadFromOBJ("Medama", true));

	// 背景モデル関連
	modelSkydome_.reset(Model::LoadFromOBJ("skydome"));
	modelGround_.reset(Model::LoadFromOBJ("ground"));

	// 木
	modelTreeBald_.reset(Model::LoadFromOBJ("tree_bald"));
	modelTreeNormal_.reset(Model::LoadFromOBJ("tree_Normal"));

	// フェンス
	modelFence_.reset(Model::LoadFromOBJ("fence"));
	modelFencePost_.reset(Model::LoadFromOBJ("fencePost"));
	modelLatticeDoor_.reset(Model::LoadFromOBJ("latticeDoor"));

	// お墓
	modelGraveCross.reset(Model::LoadFromOBJ("grave_cross"));
	modelGraveSquare.reset(Model::LoadFromOBJ("grave_square"));

#pragma endregion

#pragma region サウンド読み込み
	sound.SoundLoadWave("GameClear.wav");
#pragma endregion

#pragma region スプライト初期化
	// スプライトの初期化

	// 仮のスプライト
	spriteProvisional = std::make_unique<Sprite>();
	spriteProvisional->Initialize(textureHandleDefu.get(), { 200,200 }, { 150,150 }, { 0.5f,0.5f, 0.5f,1.0f });
	spritePos = spriteProvisional->GetPosition();

	// シーンチェンジ用のスプライト
	blackOut = std::make_unique<Sprite>();
	blackOut->Initialize(blackOutTexHandle_.get(), { WinApp::window_width / 2,WinApp::window_height / 2 }, { 1280 * 2,720 * 2 });
	blackOut->SetColor({ 1,1,1,blackAlpha });

	// タイトルの文字用のスプライト
	titleKariFont = std::make_unique<Sprite>();
	titleKariFont->Initialize(titleFontTexHandle.get(), { WinApp::window_width / 2,WinApp::window_height / 2 - 150 },{500,100});

	// タイトルのボタンAのスプライト
	titleKariPressA = std::make_unique<Sprite>();
	titleKariPressA->Initialize(titleButtonTexHandle.get(), { WinApp::window_width / 2,WinApp::window_height / 2 + 200 },{250,50});

	// タイトルの背景のスプライト
	titleKariBack = std::make_unique<Sprite>();
	titleKariBack->Initialize(titleBackTexHandle.get(), { WinApp::window_width / 2,WinApp::window_height / 2 }, { 1280,720 });

#pragma endregion

#pragma region プレイヤー関連の初期化
	player_ = std::make_unique<Player>();
	player_.reset(Player::Create(playerModel_.get()));
#pragma endregion

#pragma region コライダー本体の初期化
	gameCollider = std::make_unique<GameCollider>();
	gameCollider->Initialize();
#pragma endregion

#pragma region ローダー用の読み込み
	// レベルデータの読み込み
	levelData_.reset(LevelLoader::LoadFile("field2"));

	// モデルデータをモデルのリストに登録
	models.insert(std::make_pair("skydome", modelSkydome_.get()));
	models.insert(std::make_pair("ground", modelGround_.get()));
	models.insert(std::make_pair("chr_sword", playerModel_.get()));
	models.insert(std::make_pair("Medama", modelMedama_.get()));

	models.insert(std::make_pair("tree_bald", modelTreeBald_.get()));
	models.insert(std::make_pair("tree_Normal", modelTreeNormal_.get()));
	models.insert(std::make_pair("fence", modelFence_.get()));
	models.insert(std::make_pair("fencePost", modelFencePost_.get()));
	models.insert(std::make_pair("latticeDoor", modelLatticeDoor_.get()));
	models.insert(std::make_pair("grave_cross", modelGraveCross.get()));
	models.insert(std::make_pair("grave_square", modelGraveSquare.get()));

	// レベルデータからオブジェクトを生成、配置
	//	また、プレイヤーの初期位置やエネミーの初期
	for (auto& objectData : levelData_->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
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
			newEnemy->Initialize(modelMedama_.get(), player_.get());
			newEnemy->UpdateWorldMatrix();

			// 今作成した敵を配列に格納
			enemys_.push_back(newEnemy);
		}
#pragma endregion

#pragma region お墓のドア
		else if (objectData.tagName == "Door")
		{
			// モデルを指定して3Dオブジェクトを生成
			Object3d* newObject = Object3d::Create();
			newObject->SetModel(model);

			// 座標
			Vector3 pos;
			pos = objectData.translation;
			newObject->worldTransform_.translation = pos;

			// 回転角
			Vector3 rot;
			rot = objectData.rotation;
			newObject->worldTransform_.rotation = rot;
			newObject->worldTransform_.rotation.y += MathUtil::DegreeToRadian(180);

			// スケール
			Vector3 scale;
			scale = objectData.scaling;
			newObject->worldTransform_.scale = scale;

			// 配列に登録
			latticeDoors_.push_back(newObject);
		}
#pragma endregion


		// それ以外のタグ名またはなしの場合
		// 普通のオブジェクトと判断し生成
		else 
		{
			// モデルを指定して3Dオブジェクトを生成
			Object3d* newObject = Object3d::Create();
			newObject->SetModel(model);

			// 座標
			Vector3 pos;
			pos = objectData.translation;
			newObject->worldTransform_.translation = pos;

			// 回転角
			Vector3 rot;
			rot = objectData.rotation;
			newObject->worldTransform_.rotation = rot;

			// スケール
			Vector3 scale;
			scale = objectData.scaling;
			newObject->worldTransform_.scale = scale;

			// 配列に登録
			objects.push_back(newObject);

			// タグ名が墓もしくは木だったら、当たり判定をつける
			if (objectData.tagName == "tree"|| 
				objectData.tagName == "grave") {
				gameCollider->AddObj(newObject);
			}
		}
		
	}
#pragma endregion

#pragma region ビュープロジェクション関連の初期化
	// ビュープロジェクションの初期化
	gameCamera = std::make_unique<GameCamera>();
	gameCamera->Initialize(player_->GetCameraWorld());
	gameCamera->SetCameraFPos(player_->worldTransform_.translation);

	

	player_->SetGameCamera(gameCamera.get());
#pragma endregion

#pragma region コライダー関連の初期化


	for (auto e:enemys_){
		gameCollider->AddEnemy(e);
	}
	
	gameCollider->SetPlayer(player_.get());
#pragma endregion

#pragma region UI関連の初期化
	timerUi_ = std::make_unique<TimerUI>();
	timerUi_->Init();

	operationUi_= std::make_unique<OperationUI>();
	operationUi_->Init();

#pragma endregion

}

void GameScene::Update()
{
	// シーンチェンジ処理
	SceneChageUpdate();

	// シーンのそれぞれの更新処理
	switch (scene)
	{
	case GameScene::Scene::Title: // タイトルシーン
		TitleUpdate();
		break;
	case GameScene::Scene::Game: // ゲームシーン
		GameSceneUpdate();
		break;
	case GameScene::Scene::Result: // リザルトシーン
		ResultSceneUpdate();
		break;
	default:
		break;
	}
	

}

void GameScene::ImguiUpdate()
{

	// 表示項目の追加--------//
	ImGui::Begin("Sprite 1");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));

	ImGui::SliderFloat2("position", &spritePos.x, 0.0f, 1200.0f, "%.1f");

	//ImGui::InputFloat4("interpolate0", &interpolate0.x, "%.2f");
	//ImGui::InputFloat4("interpolate1", &interpolate1.x, "%.2f");
	//ImGui::InputFloat4("interpolate2", &interpolate2.x, "%.2f");
	//ImGui::InputFloat4("interpolate3", &interpolate3.x, "%.2f");
	//ImGui::InputFloat4("interpolate4", &interpolate4.x, "%.2f");
	//ImGui::InputFloat4("directionTodirection", &dirToDir.x, "%.2f");

	if (ImGui::Button("Reset")) {
		spritePos = { 200.0f,200.0f };
	}

	ImGui::End();

	ImGui::SetNextWindowSize(ImVec2(500, 250));
	ImGui::Begin("Light");

	// 平行光源
	if (ImGui::TreeNode("DirectionalLight")) {
		ImGui::Checkbox("Is Active", &isActiveDirectional);

		ImGui::ColorEdit3("ambientColor", &AmColor.x);
		ImGui::ColorEdit3("diffuseColor", &DiColor.x);
		ImGui::ColorEdit3("specularColor", &SpColor.x);



		ImGui::TreePop();
	}
	// ポイントライト
	if (ImGui::TreeNode("PointLight")) {
		ImGui::Checkbox("Is Active", &isActivePoint);

		if (isActivePoint == true) {
			light->SetPointLightActive(0, true);
		}
		else if (isActivePoint == false) {
			light->SetPointLightActive(0, false);
		}

		ImGui::InputFloat3("pointLightPos", pointLightPos);
		ImGui::ColorEdit3("pointLightColor", pointLightColor, ImGuiColorEditFlags_Float);
		ImGui::InputFloat3("pointLightAtten", pointLightAtten);

		ImGui::TreePop();
	}
	// スポットライト
	if (ImGui::TreeNode("spotLight")) {
		ImGui::Checkbox("Is Active", &isActiveSpot);



		ImGui::InputFloat3("spotLightPos", &spotLightPos.x);
		ImGui::InputFloat3("spotLightDir", &spotLightDir.x);
		ImGui::InputFloat3("spotLightAtten", &spotLightAtten.x);
		ImGui::ColorEdit3("spotLightColor", &spotLightColor.x, ImGuiColorEditFlags_Float);
		ImGui::InputFloat2("spotLightFactorAngle", &spotLightFactorAngle.x);

		ImGui::TreePop();
	}
	// 丸影
	if (ImGui::TreeNode("circleShadow")) {
		ImGui::Checkbox("Is Active", &isActiveCircleShadow);

		ImGui::InputFloat3("circleShadowPos", &circleShadowCasterPos.x);
		ImGui::InputFloat3("circleShadowDir", &circleShadowDir.x);
		//ImGui::ColorEdit3("spotLightColor", &spotLightColor.x, ImGuiColorEditFlags_Float);
		ImGui::InputFloat3("circleShadowAtten", &circleShadowAtten.x);
		ImGui::InputFloat2("circleShadowFactorAngle", &circleShadowFactorAngle.x);

		ImGui::TreePop();
	}
	// フォグ
	if (ImGui::TreeNode("Fog")) {
		ImGui::Checkbox("Is Active", &isFogActive);

		if (isFogActive == true) {
			fog->isActiveFog = true;
		}
		else if (isFogActive == false) {
			fog->isActiveFog = false;
		}

		ImGui::SliderFloat("nearFog", &fog->nearFog, 0.0f, 100.0f, "%.1f");
		ImGui::SliderFloat("farFog", &fog->farFog, 50.0f, 500.0f, "%.1f");
		ImGui::ColorEdit4("fogColor", &fog->fogColor.x, ImGuiColorEditFlags_Float);
		ImGui::TreePop();
	}

	ImGui::End();

	// 当たり判定-----------------------//
	//ImGui::Begin("Collision");
	//ImGui::SetNextWindowSize(ImVec2(500, 100));

	//ImGui::InputFloat3("sphere", &sphere.center.x);
	//ImGui::Text("hit:%d", hit);
	//ImGui::InputFloat3("sphere.inter", &inter.x);

	//ImGui::InputFloat3("rayStart", &ray.start.x);
	//ImGui::InputFloat3("rayDir", &ray.dir.x);
	//ImGui::Text("hitRay:%d", hitRay);
	//ImGui::InputFloat3("ray.inter", &inter.x);

	//ImGui::End();
	// ---------------------//
	ImGui::SetNextWindowSize(ImVec2(500, 100));
	ImGui::Begin("Sound");

	ImGui::Checkbox("Is Active", &isActiveSound);
	ImGui::Checkbox("Is Stop", &isStopSound);

	ImGui::End();
}

void GameScene::Draw2DBack()
{
	switch (scene)
	{
	case GameScene::Scene::Title: // タイトルシーン

		break;
	case GameScene::Scene::Game: // ゲームシーン

		break;
	case GameScene::Scene::Result: // リザルトシーン

		break;
	default:
		break;
	}
}

void GameScene::Draw3D()
{
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	switch (scene)
	{
	case GameScene::Scene::Title: // タイトルシーン

		// お墓のドアのオブジェクトの描画
		for (auto l: latticeDoors_){
			l->Draw(&gameCamera->GetView());
		}

		// 普通のオブジェクトの描画
		for (auto o : objects) {
			o->Draw(&gameCamera->GetView());
		}

		break;
	case GameScene::Scene::Game: // ゲームシーン

		// オブジェクト関連の描画

		// プレイヤーの描画
		player_->Draw(&gameCamera->GetView());

		// 敵の描画
		for (auto e : enemys_) {
			e->Draw(&gameCamera->GetView());
		}

		// お墓のドアのオブジェクトの描画
		for (auto l : latticeDoors_){
			l->Draw(&gameCamera->GetView());
		}

		// 普通のオブジェクトの描画
		for (auto o : objects) {
			o->Draw(&gameCamera->GetView());
		}
		gameCollider->Draw3D(&gameCamera->GetView());
		// FBXモデルの描画
		FbxModel::PreDraw(commandList);


		FbxModel::PostDraw();

		ParticleManager::PreDraw(commandList);
		// パーティクルの描画
		DrawParticle();
		// 敵の描画
		for (auto e : enemys_) {
			e->DrawParticle(&gameCamera->GetView());
		}
		ParticleManager::PostDraw();
		break;
	case GameScene::Scene::Result: // リザルトシーン

		break;
	default:
		break;
	}
}

void GameScene::DrawParticle()
{
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	gameCollider->Draw(commandList, &gameCamera->GetView());
}

void GameScene::Draw2DFront()
{
	switch (scene)
	{
	case GameScene::Scene::Title: // タイトルシーン
		//titleKariBack->Draw();
		titleKariFont->Draw();
		titleKariPressA->Draw();
		
		break;
	case GameScene::Scene::Game: // ゲームシーン
		//spriteProvisional->Draw();
		player_->Draw2DFront();
		timerUi_->DrawFrontSprite();
		operationUi_->DrawFrontSprite();
		break;
	case GameScene::Scene::Result: // リザルトシーン

		break;
	default:
		break;
	}

	// シーンチェンジ用ののスプライトの描画
	blackOut->Draw();
}

void GameScene::Reset()
{
	switch (scene)
	{
	case GameScene::Scene::Title:
		break;
	case GameScene::Scene::Game:

		break;
	case GameScene::Scene::Result:

		// ウェーブのリセット
		waveTimeNum_ = 0;
		waveTimer_ = 0;

		// ゲームコライダーのリセット
		gameCollider->Reset();

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

		break;
	default:
		break;
	}
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
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
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
			newEnemy->Initialize(modelMedama_.get(), player_.get());
			newEnemy->UpdateWorldMatrix();

			// 今作成した敵を配列に格納
			enemys_.push_back(newEnemy);
			// 今作成した敵を当たり判定に情報を登録
			gameCollider->AddEnemy(newEnemy);
		}
#pragma endregion
	}
}

void GameScene::TitleUpdate()
{
	// パッドでAボタンを押すか、もしくはスペースキーを押した瞬間
	if (JoypadInput::GetButtonDown(PadCode::ButtonA)||input_->TriggerKey(DIK_SPACE)) {
		gameCamera->SetIsCanEase(true);
	}

#pragma region ライトの更新処理
	isActiveCircleShadow = false;
	light->SetPointLightPos(0, Vector3(pointLightPos[0], pointLightPos[1], pointLightPos[2]));
	light->SetPointLightColor(0, Vector3(pointLightColor[0], pointLightColor[1], pointLightColor[2]));
	light->SetPointLightAtten(0, Vector3(pointLightAtten[0], pointLightAtten[1], pointLightAtten[2]));

	light->SetSpotLightDir(0, spotLightDir);
	light->SetSpotLightPos(0, spotLightPos);
	light->SetSpotLightColor(0, spotLightColor);
	light->SetSpotLightAtten(0, spotLightAtten);
	light->SetSpotLightFactorAngle(0, spotLightFactorAngle);

	light->SetCircleShadowDir(0, circleShadowDir);

	light->SetCircleShadowAtten(0, circleShadowAtten);
	light->SetCircleShadowFactorAngle(0, circleShadowFactorAngle);

	light->SetAmbientColor(AmColor);
	light->SetDiffuseColor(DiColor);
	light->SetSpecularColor(SpColor);

	if (isActiveDirectional == true) {
		light->SetDirLightActive(0, true);
	}
	else if (isActiveDirectional == false) {
		light->SetDirLightActive(0, false);
	}
	if (isActiveSpot == true) {
		light->SetSpotLightActive(0, true);
	}
	else if (isActiveSpot == false) {
		light->SetSpotLightActive(0, false);
	}
	if (isActiveCircleShadow == true) {
		light->SetCircleShadowActive(0, true);
	}
	else if (isActiveCircleShadow == false) {
		light->SetCircleShadowActive(0, false);
	}
#pragma endregion

	// オブジェクトの更新処理
	for (auto o : objects) {
		o->Update();
	}

	// お墓のドアのオブジェクトの描画
	if (gameCamera->GetIsCanEase() == true) {
		latticeDoors_[0]->worldTransform_.rotation.y -= MathUtil::DegreeToRadian(2);
		latticeDoors_[1]->worldTransform_.rotation.y += MathUtil::DegreeToRadian(2);
	}

	for (auto l : latticeDoors_)
	{
		l->Update();
	}

	// カメラの更新処理
	//gameCamera->GetView().eye.z += 0.1f;
	gameCamera->TitleUpdate();

	if (gameCamera->GetIsEaseEnd() == true) {
		oldScene = Scene::Title;
		sceneChangeFlag = true;
	}

	light->Update();
}

void GameScene::GameSceneUpdate()
{
	// 入力の更新
	input_->Update();

	// 数字の0キーが押されていたら
	if (input_->PushKey(DIK_0))
	{
		OutputDebugStringA("Hit 0\n");  // 出力ウィンドウに「Hit 0」と表示
	}

#pragma region ライトの更新処理
	light->SetPointLightPos(0, Vector3(pointLightPos[0], pointLightPos[1], pointLightPos[2]));
	light->SetPointLightColor(0, Vector3(pointLightColor[0], pointLightColor[1], pointLightColor[2]));
	light->SetPointLightAtten(0, Vector3(pointLightAtten[0], pointLightAtten[1], pointLightAtten[2]));

	light->SetSpotLightDir(0, spotLightDir);
	light->SetSpotLightPos(0, spotLightPos);
	light->SetSpotLightColor(0, spotLightColor);
	light->SetSpotLightAtten(0, spotLightAtten);
	light->SetSpotLightFactorAngle(0, spotLightFactorAngle);

	light->SetCircleShadowDir(0, circleShadowDir);

	light->SetCircleShadowAtten(0, circleShadowAtten);
	light->SetCircleShadowFactorAngle(0, circleShadowFactorAngle);

	light->SetAmbientColor(AmColor);
	light->SetDiffuseColor(DiColor);
	light->SetSpecularColor(SpColor);

	if (isActiveDirectional == true) {
		light->SetDirLightActive(0, true);
	}
	else if (isActiveDirectional == false) {
		light->SetDirLightActive(0, false);
	}
	if (isActiveSpot == true) {
		light->SetSpotLightActive(0, true);
	}
	else if (isActiveSpot == false) {
		light->SetSpotLightActive(0, false);
	}
	if (isActiveCircleShadow == true) {
		light->SetCircleShadowActive(0, true);
	}
	else if (isActiveCircleShadow == false) {
		light->SetCircleShadowActive(0, false);
	}
#pragma endregion

	spriteProvisional->SetPosition(spritePos);

	fog->UpdateMatrix();
	Object3d::SetFog(fog.get());

	if (isActiveSound == true) {
		sound.SoundPlayWave(true, 0.01f);
		isActiveSound = false;
	}
	if (isStopSound == true) {
		sound.StopWave();
		isStopSound = false;
	}

#pragma region プレイヤーの更新処理
	// プレイヤーの更新処理
	player_->Update();

	// プレイヤーが死んだらとりあえずリザルト画面に移行
	if (player_->GetIsAlive() == false) {
		oldScene = Scene::Game;
		sceneChangeFlag = true;
	}
#pragma endregion


#pragma region 敵の更新処理関連
	
	EnemyGameUpdate();
#pragma endregion

	
	// カメラの更新処理
	gameCamera->SetCameraPos(player_->worldTransform_.translation);
	gameCamera->GameUpdate();

	// お墓のドアのオブジェクトの描画
	for (auto l : latticeDoors_){
		l->Update();
	}

	// オブジェクトの更新処理
	for (auto o : objects) {
		o->Update();
	}

	// 当たり判定関連の更新処理
	gameCollider->Updata();

	// ライトの更新処理
	circleShadowCasterPos = player_->GetWorldPosition();
	light->SetCircleShadowCasterPos(0, circleShadowCasterPos);
	light->Update();


	// タイマーの更新処理
	timerUi_->Update();

	// オペレーションの更新処理
	operationUi_->Update();

	//// 敵が全滅したらとりあえずシーンを切り替える
	//if (enemys_.size() <= 1) {
	//	oldScene = Scene::Game;
	//	sceneChangeFlag = true;
	//}
}

void GameScene::ResultSceneUpdate()
{
	// パッドでAボタンを押すか、もしくはスペースキーを押した瞬間
	if (JoypadInput::GetButtonDown(PadCode::ButtonA) || input_->TriggerKey(DIK_SPACE)) {
		oldScene = Scene::Result;
		sceneChangeFlag = true;
		resultChange = true;
	}
}

void GameScene::SceneChageUpdate()
{
	if (sceneChangeFlag == true) {
		switch (scene)
		{
		case GameScene::Scene::Title:
			if (oldScene == Scene::Title) {
				blackAlpha += 0.025f;
				blackOut->SetColor({ 1,1,1,blackAlpha });
				if (blackAlpha >= 1) {
					blackAlpha = 1;
					scene = Scene::Game;
					// ここにリセット関数を置く
				}
			}
			else if (oldScene == Scene::Result) {
				blackAlpha -= 0.025f;
				blackOut->SetColor({ 1,1,1,blackAlpha });
				if (blackAlpha <= 0) {
					blackAlpha = 0;
					sceneChangeFlag = false;
				}
			}
			break;
		case GameScene::Scene::Game:
			if (oldScene != Scene::Game) {
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
					// ここにリセット関数を置く
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
						// ここにリセット関数を置く
					}
					else if (resultChange == true) {
						// ここにリセット関数を置く
						Reset();

						scene = Scene::Title;
						
						
					}
				}
			}
			break;
		default:
			break;
		}
	}
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
}
