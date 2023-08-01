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

GameScene::~GameScene()
{
	
}

void GameScene::Initialize()
{
	srand((unsigned int)time(NULL));
	// 入力の初期化
	input_ = std::make_unique<Input>();
	input_->Initialize();

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
	modelSkydome.reset(Model::LoadFromOBJ("skydome"));
	modelGround.reset(Model::LoadFromOBJ("ground"));
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

#pragma region ローダー用の読み込み
	// レベルデータの読み込み
	levelData_.reset(LevelLoader::LoadFile("testScene"));


	// オブジェクトの初期化
	objSkydome.reset(Object3d::Create());
	objGround.reset(Object3d::Create());

	objSkydome->SetModel(modelSkydome.get());
	objGround->SetModel(modelGround.get());

	// モデルデータをモデルのリストに登録
	models.insert(std::make_pair("skydome", modelSkydome.get()));
	models.insert(std::make_pair("ground", modelGround.get()));
	models.insert(std::make_pair("chr_sword", playerModel_.get()));
	models.insert(std::make_pair("sphere", modelMedama_.get()));

	// レベルデータからオブジェクトを生成、配置
	//	また、プレイヤーの初期位置やエネミーの初期
	for (auto& objectData : levelData_->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

		// タグ名がプレイヤーなら
		if (objectData.tagName == "player")
		{
			WorldTransform w;
			w.Initialize();
			w.translation = objectData.translation;
			w.scale = objectData.scaling;
			w.rotation = objectData.rotation;


			player_->SetWorldTransInfo(w);
			player_->UpdateWorldMatrix();
		}
		// タグ名がエネミーなら
		else if (objectData.tagName == "enemy" && player_ != nullptr)
		{
			WorldTransform w;
			w.Initialize();
			w.translation = objectData.translation;
			w.scale = objectData.scaling;
			w.rotation = objectData.rotation;

			// 新しい敵の生成
			Enemy* newEnemy;
			newEnemy->Initialize(modelMedama_.get(), player_.get());

		}
		// それ以外のタグ名またはなしの場合
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
		}
		
	}
#pragma endregion

#pragma region エネミー関連の初期化
	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(modelMedama_.get(), player_.get());
#pragma endregion

#pragma region ビュープロジェクション関連の初期化
	// ビュープロジェクションの初期化
	gameCamera = std::make_unique<GameCamera>();
	gameCamera->Initialize(player_->GetCameraWorld());
	player_->SetGameCamera(gameCamera.get());
#pragma endregion

#pragma region コライダー関連の初期化
	gameCollider = std::make_unique<GameCollider>();
	gameCollider->Initialize();
	gameCollider->AddEnemy(enemy_.get());
	gameCollider->SetPlayer(player_.get());
#pragma endregion

	objMedama_.reset(Object3d::Create());
	objMedama_->SetModel(modelMedama_.get());
	objMedama_->worldTransform_.translation = { 2,1,0 };

	objMedama_->dissolve_.isActiveDissolve_ = true;
	objMedama_->Update();

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

		if (isActiveDirectional == true) {
			light->SetDirLightActive(0, true);
		}
		else if (isActiveDirectional == false) {
			light->SetDirLightActive(0, false);
		}

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

		if (isActiveSpot == true) {
			light->SetSpotLightActive(0, true);
		}
		else if (isActiveSpot == false) {
			light->SetSpotLightActive(0, false);
		}

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

		if (isActiveCircleShadow == true) {
			light->SetCircleShadowActive(0, true);
		}
		else if (isActiveCircleShadow == false) {
			light->SetCircleShadowActive(0, false);
		}

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

	// フォグ
	if (ImGui::TreeNode("Dissolve")) {
		ImGui::Checkbox("Is Active", &isDissolve);

		if (isDissolve == true) {
			objMedama_->dissolve_.isActiveDissolve_ = true;
		}
		else if (isDissolve == false) {
			objMedama_->dissolve_.isActiveDissolve_ = false;
		}

		ImGui::SliderFloat("dissolvePower", &objMedama_->dissolve_.dissolvePower_, 1.0f, 50.0f, "%.1f");
		ImGui::SliderFloat("dissolveTime", &objMedama_->dissolve_.dissolveTime_, 0.0f, 2.0f, "%.1f");
		ImGui::SliderFloat("dissolve", &objMedama_->dissolve_.dissolveSmoothMin_, -1.0f, -0.1f, "%.1f");
		ImGui::ColorEdit4("fogColor", &objMedama_->dissolve_.dissolveColor_.x, ImGuiColorEditFlags_Float);
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

		break;
	case GameScene::Scene::Game: // ゲームシーン

		// オブジェクト関連の描画
		objSkydome->Draw(&gameCamera->GetView());
		objGround->Draw(&gameCamera->GetView());

		player_->Draw(&gameCamera->GetView());

		enemy_->Draw(&gameCamera->GetView());

		objMedama_->Draw(&gameCamera->GetView());

		// FBXモデルの描画
		FbxModel::PreDraw(commandList);


		FbxModel::PostDraw();

		// パーティクルの描画
		DrawParticle();
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
		titleKariBack->Draw();
		titleKariFont->Draw();
		titleKariPressA->Draw();
		
		break;
	case GameScene::Scene::Game: // ゲームシーン
		spriteProvisional->Draw();
		break;
	case GameScene::Scene::Result: // リザルトシーン

		break;
	default:
		break;
	}

	// シーンチェンジ用ののスプライトの描画
	blackOut->Draw();
}

void GameScene::TitleUpdate()
{
	// パッドでAボタンを押すか、もしくはスペースキーを押した瞬間
	if (JoypadInput::GetButtonDown(PadCode::ButtonA)||input_->TriggerKey(DIK_SPACE)) {
		oldScene = Scene::Title;
		sceneChangeFlag = true;
	}
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

	//objFighter->worldTransform_.position_ = fighterPos;

	

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


	//for (auto& object : objects) {
	//	object->Update();
	//}
	//frem += 0.01f;
	//fbxmodel_->ModelAnimation(frem, modelAnim_->GetAnimation(static_cast<int>(0)), BoneNum);

	player_->Update();

	enemy_->Update();

	gameCamera->SetCameraPos(player_->worldTransform_.translation);
	gameCamera->Update();

	objSkydome->Update();
	objGround->Update();

	objMedama_->Update();

	// 当たり判定関連の更新処理
	gameCollider->Updata();

	circleShadowCasterPos = player_->GetWorldPosition();
	light->SetCircleShadowCasterPos(0, circleShadowCasterPos);
	light->Update();
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
						scene = Scene::Title;
						// ここにリセット関数を置く
					}
				}
			}
			break;
		default:
			break;
		}
	}
}
