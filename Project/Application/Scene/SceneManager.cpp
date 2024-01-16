#include "SceneManager.h"
#include "BeatEffect.h"

SceneManager::~SceneManager()
{
	models.clear();

	for (auto o : objects) {
		delete o;
	}
	objects.clear();

	for (auto o : latticeDoors_) {
		delete o;
	}
	latticeDoors_.clear();

	for (auto o : ranterns_) {
		delete o;
	}
	ranterns_.clear();

	for (auto o : highLumiRanterns_) {
		delete o;
	}
	highLumiRanterns_.clear();

	for (auto o : pointLightsInfo_) {
		delete o;
	}
	pointLightsInfo_.clear();

	Enemy::StaticFinalize();
}

void SceneManager::Initialize()
{
	Enemy::StaticInitialize();

#pragma region ライトの初期化
	// ライトの生成
	light.reset(LightGroup::Create());
	// ライト設定
	light->SetDirLightActive(0, true);
	light->SetDirLightActive(1, false);
	light->SetDirLightActive(2, false);

	light->SetSpotLightActive(0, false);

	light->SetCircleShadowActive(0, true);

	// 3Dオブジェクトにライトをセット
	Object3d::SetLight(light.get());
#pragma endregion

#pragma region フォグ
	fog.reset(Fog::Create());
	fog->nearFog = 20;
	fog->farFog = 150;
	fog->isActiveFog = true;
	fog->fogColor = Vector4(0.08f, 0.08f, 0.152f, 1);
	Object3d::SetFog(fog.get());
#pragma endregion

#pragma region モデルの読み込み

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

	// ランタン
	modelWallRantern_.reset(Model::LoadFromOBJ("WallRantern"));
	modelPostRantern_.reset(Model::LoadFromOBJ("postRantern"));

	// 壁
	modelWall02_.reset(Model::LoadFromOBJ("wall02"));
	modelWall03_.reset(Model::LoadFromOBJ("wall03"));

	// 椅子
	modelChair_.reset(Model::LoadFromOBJ("chair"));

	// 十字架
	modelCross_.reset(Model::LoadFromOBJ("cross"));
	modelStoneCross_.reset(Model::LoadFromOBJ("stoneCross"));

#pragma endregion

#pragma region テクスチャの読み込み
	// シーンチェンジで使うハンドル
	blackOutTexHandle_.reset(TextureManager::Load2DTextureP("SceneChageTex.png"));
#pragma endregion

#pragma region スプライトの初期化
	// シーンチェンジ用のスプライト
	blackOut = std::make_unique<Sprite>();
	blackOut->Initialize(blackOutTexHandle_.get(), { WinApp::window_width / 2,WinApp::window_height / 2 }, { 1280 * 2,720 * 2 });
	blackOut->SetColor({ 1,1,1,blackAlpha });
#pragma endregion

#pragma region コライダー本体の初期化
	gameCollider = std::make_unique<GameCollider>();
	gameCollider->Initialize();
#pragma endregion

	InitLoader();

#pragma region ポイントライトのセット
	for (size_t i = 0; i < pointLightsInfo_.size(); i++)
	{
		// ポイントライトのフラグをアクティブに
		light->SetPointLightActive((int)i, true);

		// ポジションの情報をセット
		light->SetPointLightPos((int)i, pointLightsInfo_[i]->translation);

		// ポイントライトのライトの減衰具合
		light->SetPointLightAtten((int)i, pointLightAtten);

		// ポイントライトのライトの色
		light->SetPointLightColor((int)i, pointLightColor);
	}

	// プレイヤーのポイントライトをオン
	//light->SetPointLightActive((int)(pointLightsInfo_.size() + 1), true);
#pragma endregion

#pragma region ビュープロジェクション関連の初期化
	// ビュープロジェクションの初期化
	gameCamera = std::make_unique<GameCamera>();
	gameCamera->Initialize();
#pragma endregion

#pragma region 鼓動エフェクトの初期化

	BeatEffect::SetCmdList(dxCommon_->GetCommandList());
#pragma endregion

#pragma region シーンの初期化
	titleScene_ = std::make_unique<TitleScene>();
	gameScene_ = std::make_unique<GameScene>();
	gameClearScene_ = std::make_unique<GameClearScene>();
	gameOverScene_ = std::make_unique<GameOverScene>();

	InitScenesSets();

	titleScene_->Initialize();
	gameScene_->Initialize();
	gameClearScene_->Initialize();
	gameOverScene_->Initialize();
#pragma endregion

}

void SceneManager::Update()
{
	// シーンチェンジ処理
	SceneChageUpdate();

	switch (scene_)
	{
	case SceneManager::Title:
		titleScene_->Update();
		break;
	case SceneManager::Game:
		gameScene_->Update();
		break;
	case SceneManager::GameClear:
		gameClearScene_->Update();
		break;
	case SceneManager::GameOver:
		gameOverScene_->Update();
		break;
	default:
		break;
	}

	LightUpdate();
	fog->UpdateMatrix();
}

void SceneManager::ImguiUpdate()
{
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
		//ImGui::Checkbox("Is Active", &isActivePoint);

		ImGui::InputFloat3("pointLightPos", pointLightPos);
		ImGui::ColorEdit3("pointLightColor", &pointLightColor.x, ImGuiColorEditFlags_Float);
		ImGui::SliderFloat3("pointLightAtten", &pointLightAtten.x, 0, 1);

		ImGui::InputFloat("pointLightIndensity", &pointLightIndensity);
		ImGui::InputFloat("pointLightRadius", &pointLightRadius);
		ImGui::InputFloat("pointLightDecay", &pointLightDecay);
		ImGui::InputFloat("pointLightDistance", &pointLightDistance);

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

	switch (scene_)
	{
	case SceneManager::Title:
		break;
	case SceneManager::Game:
		gameScene_->ImguiUpdate();
		break;
	case SceneManager::GameClear:

		break;
	case SceneManager::GameOver:

		break;
	default:
		break;
	}
}

void SceneManager::Draw2DBack()
{
}

void SceneManager::Draw3D()
{
	switch (scene_)
	{
	case SceneManager::Title:
		titleScene_->Draw3D();
		break;
	case SceneManager::Game:
		gameScene_->Draw3D();
		break;
	case SceneManager::GameClear:
		gameClearScene_->Draw3D();
		break;
	case SceneManager::GameOver:
		gameOverScene_->Draw3D();
		break;
	default:
		break;
	}

	DrawParticle();
}

void SceneManager::DrawParticle()
{
	switch (scene_)
	{
	case SceneManager::Title:

		break;
	case SceneManager::Game:
		gameScene_->DrawParticle();
		break;
	case SceneManager::GameClear:

		break;
	case SceneManager::GameOver:

		break;
	default:
		break;
	}
}

void SceneManager::Draw2DFront()
{
	switch (scene_)
	{
	case SceneManager::Title:
		titleScene_->Draw2DFront();
		break;
	case SceneManager::Game:
		gameScene_->Draw2DFront();
		break;
	case SceneManager::GameClear:
		gameClearScene_->Draw2DFront();
		break;
	case SceneManager::GameOver:
		gameOverScene_->Draw2DFront();
		break;
	default:
		break;
	}

	// シーンチェンジ用ののスプライトの描画
	blackOut->Draw();
}

void SceneManager::DrawBloomObject()
{
	switch (scene_)
	{
	case SceneManager::Title:
		titleScene_->DrawBloomObject();
		break;
	case SceneManager::Game:
		gameScene_->DrawBloomObject();
		break;
	case SceneManager::GameClear:

		break;
	case SceneManager::GameOver:

		break;
	default:
		break;
	}
}

void SceneManager::DrawHighLumiObj()
{
	switch (scene_)
	{
	case SceneManager::Title:
		titleScene_->DrawHighLumiObj();
		break;
	case SceneManager::Game:
		gameScene_->DrawHighLumiObj();
		break;
	case SceneManager::GameClear:

		break;
	case SceneManager::GameOver:

		break;
	default:
		break;
	}
}

void SceneManager::SetBeatEffect(BeatEffect* beatEffect)
{
	gameScene_->SetBeatEffect(beatEffect);
}

void SceneManager::InitLoader()
{
#pragma region ローダー用の読み込み
	// レベルデータの読み込み
	levelData_.reset(LevelLoader::LoadFile("testS"));

	// モデルデータをモデルのリストに登録
	models.insert(std::make_pair("skydome", modelSkydome_.get()));
	models.insert(std::make_pair("ground", modelGround_.get()));
	models.insert(std::make_pair("Medama", modelMedama_.get()));

	models.insert(std::make_pair("tree_bald", modelTreeBald_.get()));
	models.insert(std::make_pair("tree_Normal", modelTreeNormal_.get()));
	models.insert(std::make_pair("fence", modelFence_.get()));
	models.insert(std::make_pair("fencePost", modelFencePost_.get()));
	models.insert(std::make_pair("latticeDoor", modelLatticeDoor_.get()));
	models.insert(std::make_pair("grave_cross", modelGraveCross.get()));
	models.insert(std::make_pair("grave_square", modelGraveSquare.get()));
	models.insert(std::make_pair("WallRantern", modelWallRantern_.get()));

	models.insert(std::make_pair("postRantern", modelPostRantern_.get()));
	models.insert(std::make_pair("wall02", modelWall02_.get()));
	models.insert(std::make_pair("wall03", modelWall03_.get()));
	models.insert(std::make_pair("cross", modelCross_.get()));
	models.insert(std::make_pair("stoneCross", modelStoneCross_.get()));
	models.insert(std::make_pair("chair", modelChair_.get()));

	// レベルデータからオブジェクトを生成、配置
	//	また、プレイヤーの初期位置やエネミーの初期
	for (auto& objectData : levelData_->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

		if (objectData.tagName == "enemy") {

		}
		else if (objectData.tagName == "ItemKey") {

		}
		// タグ名がドアなら
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

		// タグ名がランタンなら
		else if (objectData.tagName == "Rantern")
		{
			// モデルを指定して3Dオブジェクトを生成
			Object3d* newObject = Object3d::Create();
			Object3d* newObj2 = Object3d::Create();
			newObject->SetModel(model);
			newObj2->SetModel(model);

			// 座標
			Vector3 pos;
			pos = objectData.translation;
			newObject->worldTransform_.translation = pos;
			newObj2->worldTransform_.translation = pos;

			// 回転角
			Vector3 rot;
			rot = objectData.rotation;
			newObject->worldTransform_.rotation = rot;
			newObj2->worldTransform_.rotation = rot;
			//newObject->worldTransform_.rotation.y += MathUtil::DegreeToRadian(180);

			// スケール
			Vector3 scale;
			scale = objectData.scaling;
			newObject->worldTransform_.scale = scale;
			newObj2->worldTransform_.scale = scale;

			// 配列に登録
			ranterns_.push_back(newObject);
			highLumiRanterns_.push_back(newObj2);
		}

		// タグ名がポイントライトなら
		else if (objectData.tagName == "PointLight")
		{
			// ポイントライトの登録
			WorldTransform* newWorldTrans = new WorldTransform();
			newWorldTrans->Initialize();

			newWorldTrans->translation = objectData.translation;

			// 配列に登録
			pointLightsInfo_.push_back(newWorldTrans);

		}

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
			if (objectData.tagName == "tree" ||
				objectData.tagName == "grave") {
				gameCollider->AddObj(newObject);
			}
		}

	}
#pragma endregion
}

void SceneManager::InitScenesSets()
{
	titleScene_->SetGameCamera(gameCamera.get());
	titleScene_->SetObjs(objects, TitleScene::Normal);
	titleScene_->SetObjs(latticeDoors_, TitleScene::Doors);
	titleScene_->SetObjs(ranterns_, TitleScene::Ranterns);
	titleScene_->SetObjs(highLumiRanterns_, TitleScene::HiguLumiRanterns);
	titleScene_->SetLightGroup(light.get());
	titleScene_->SetPointInfo(pointLightsInfo_);

	gameScene_->SetCmdList(dxCommon_->GetCommandList());
	gameScene_->SetGameCamera(gameCamera.get());
	gameScene_->SetGameCollider(gameCollider.get());
	gameScene_->SetModels(models);
	gameScene_->SetObjs(objects, GameScene::Normal);
	gameScene_->SetObjs(latticeDoors_, GameScene::Doors);
	gameScene_->SetObjs(ranterns_, GameScene::Ranterns);
	gameScene_->SetObjs(highLumiRanterns_, GameScene::HiguLumiRanterns);
	gameScene_->SetLightGroup(light.get());
	gameScene_->SetPointInfo(pointLightsInfo_);

}

void SceneManager::LightUpdate()
{
	for (size_t i = 0; i < pointLightsInfo_.size(); i++)
	{
		// ポイントライトのフラグをアクティブに
		light->SetPointLightActive((int)i, true);

		// ポイントライトのライトの減衰具合
		light->SetPointLightAtten((int)i, pointLightAtten);

		// ポイントライトのライトの色
		light->SetPointLightColor((int)i, pointLightColor);

		light->SetPointLightIndensity((int)i, pointLightIndensity);
		light->SetPointLightRadius((int)i, pointLightRadius);
		light->SetPointLightDecay((int)i, pointLightDecay);
	}

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
		//light->SetSpotLightActive(0, true);
	}
	else if (isActiveSpot == false) {
		light->SetSpotLightActive(0, false);
	}
	if (isActiveCircleShadow == true) {
		//light->SetCircleShadowActive(0, true);
	}
	else if (isActiveCircleShadow == false) {
		light->SetCircleShadowActive(0, false);
	}

	// ライトの更新処理
	light->Update();
}

void SceneManager::SceneChageUpdate()
{

	if (titleScene_->GetIsSceneFinsh() == true) {
		oldScene = Scene::Title;
		sceneChangeFlag = true;
	}
	else if(gameScene_->GetIsSceneFinsh() == true) {
		oldScene = Scene::Game;
		sceneChangeFlag = true;
	}
	else if (gameClearScene_->GetIsSceneFinsh() == true) {
		oldScene = Scene::GameClear;
		sceneChangeFlag = true;
	}
	else if (gameOverScene_->GetIsSceneFinsh() == true) {
		oldScene = Scene::GameOver;
		sceneChangeFlag = true;
	}

	if (sceneChangeFlag == false) {
		return;
	}

	switch (scene_)
	{
	case Scene::Title:
		if (titleScene_->GetIsSceneFinsh()==true) {
			blackAlpha += 0.025f;
			blackOut->SetColor({ 1,1,1,blackAlpha });
			if (blackAlpha >= 1) {
				// ここにリセット関数を置く
				Reset();
				blackAlpha = 1;
				scene_ = Scene::Game;

			}
		}
		else if (oldScene == Scene::GameClear ||
			oldScene == Scene::GameOver) {
			blackAlpha -= 0.025f;
			blackOut->SetColor({ 1,1,1,blackAlpha });
			if (blackAlpha <= 0) {
				blackAlpha = 0;
				sceneChangeFlag = false;
			}
		}
		break;
	case Scene::Game:
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
				// ここにリセット関数を置く
				
				if (gameScene_->GetIsGameClear() == true) {
					Reset();
					scene_ = Scene::GameClear;
				}
				else {
					Reset();
					scene_ = Scene::GameOver;
				}
			}
		}
		break;
	case Scene::GameClear:
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

				if (gameClearScene_->GetIsTitleOrGame() == false) {
					scene_ = Scene::Game;
					// ここにリセット関数を置く
				}
				else if (gameClearScene_->GetIsTitleOrGame() == true) {
					// ここにリセット関数を置く
					Reset();

					scene_ = Scene::Title;
				}
			}
		}
		break;
	case Scene::GameOver:
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

				if (gameOverScene_->GetIsTitleOrGame() == false) {
					scene_ = Scene::Game;
					// ここにリセット関数を置く
				}
				else if (gameOverScene_->GetIsTitleOrGame() == true) {
					// ここにリセット関数を置く
					Reset();

					scene_ = Scene::Title;
				}
			}
		}
		break;
	default:
		break;
	}

}

void SceneManager::Reset()
{
	switch (scene_)
	{
	case SceneManager::Title:
		titleScene_->Reset();
		break;
	case SceneManager::Game:
		gameScene_->Reset();
		break;
	case SceneManager::GameClear:
		gameClearScene_->Reset();
		break;
	case SceneManager::GameOver:
		gameOverScene_->Reset();
		break;
	default:
		break;
	}
}
