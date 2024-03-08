#include "SceneManager.h"
#include "BeatEffect.h"
#include"LoadManager.h"
#include "DeferredRender.h"
SceneManager::~SceneManager()
{
	loadManager_->Finalize();
	loadManager_ = nullptr;
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
	// ディファードレンダリングにライトをセット
	DeferredRender::SetLight(light.get());
#pragma endregion

#pragma region フォグ
	fog.reset(Fog::Create());
	fog->nearFog = 20;
	fog->farFog = 150;
	fog->isActiveFog = true;
	fog->fogColor = Vector4(0.08f, 0.08f, 0.152f, 1);
	Object3d::SetFog(fog.get());
	DeferredRender::SetFog(fog.get());
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

#pragma region ロードマネージャー
	loadManager_ = std::make_unique<LoadManager>();
	loadManager_->SetCollider(gameCollider.get());
	loadManager_->LoadAll();
#pragma endregion

#pragma region ポイントライトのセット
	for (size_t i = 0; i < loadManager_->GetPointLightInfo().size(); i++)
	{
		// ポイントライトのフラグをアクティブに
		light->SetPointLightActive((int)i, true);

		// ポジションの情報をセット
		light->SetPointLightPos((int)i, loadManager_->GetPointLightInfo()[i]->translation);

		// ポイントライトのライトの減衰具合
		light->SetPointLightAtten((int)i, pointLightAtten);

		// ポイントライトのライトの色
		light->SetPointLightColor((int)i, pointLightColor);
	}
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

	currentScene_ = titleScene_.get();
#pragma endregion

}

void SceneManager::Update()
{
	// シーンチェンジ処理
	SceneChageUpdate();

	// 現在のシーンの更新処理
	currentScene_->Update();

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

	currentScene_->ImguiUpdate();
}

void SceneManager::Draw2DBack()
{
}

void SceneManager::DrawForward3D()
{
	currentScene_->DrawForward3D();
}

void SceneManager::DrawDefrred3D()
{
	currentScene_->DrawDefrred3D();
}

void SceneManager::DrawParticle()
{

}

void SceneManager::Draw2DFront()
{
	currentScene_->Draw2DFront();

	// シーンチェンジ用ののスプライトの描画
	blackOut->Draw();
}

void SceneManager::DrawBloomObject()
{
	currentScene_->DrawBloomObject();
}

void SceneManager::DrawHighLumiObj()
{
	currentScene_->DrawHighLumiObj();
}

void SceneManager::SetBeatEffect(BeatEffect* beatEffect)
{
	gameScene_->SetBeatEffect(beatEffect);
}

void SceneManager::SetNextScene(SceneType sceneType)
{
	// 最初に今のシーンをリセット
	currentScene_->Reset();

	// シーンのタイプごとにシーンを変える
	switch (sceneType)
	{
	case Title:
		currentScene_ = titleScene_.get();
		sceneType_ = Title;
		break;
	case Game:
		currentScene_ = gameScene_.get();
		sceneType_ = Game;
		break;
	case GameClear:
		currentScene_ = gameClearScene_.get();
		sceneType_ = GameClear;
		break;
	case GameOver:
		currentScene_ = gameOverScene_.get();
		sceneType_ = GameOver;
		break;
	default:
		break;
	}
}

void SceneManager::InitScenesSets()
{
	titleScene_->SetGameCamera(gameCamera.get());
	titleScene_->SetObjs(loadManager_->GetObjects(), TitleScene::Normal);
	titleScene_->SetObjs(loadManager_->GetLatticeDoors(), TitleScene::Doors);
	titleScene_->SetObjs(loadManager_->GetRanterns(), TitleScene::Ranterns);
	titleScene_->SetObjs(loadManager_->GetHighRanterns(), TitleScene::HiguLumiRanterns);
	titleScene_->SetLightGroup(light.get());
	titleScene_->SetPointInfo(loadManager_->GetPointLightInfo());

	gameScene_->SetCmdList(dxCommon_->GetCommandList());
	gameScene_->SetGameCamera(gameCamera.get());
	gameScene_->SetGameCollider(gameCollider.get());
	gameScene_->SetModels(loadManager_->GetModels());
	gameScene_->SetObjs(loadManager_->GetObjects(), GameScene::Normal);
	gameScene_->SetObjs(loadManager_->GetLatticeDoors(), GameScene::Doors);
	gameScene_->SetObjs(loadManager_->GetRanterns(), GameScene::Ranterns);
	gameScene_->SetObjs(loadManager_->GetHighRanterns(), GameScene::HiguLumiRanterns);
	gameScene_->SetLightGroup(light.get());
	gameScene_->SetPointInfo(loadManager_->GetPointLightInfo());

}

void SceneManager::LightUpdate()
{
	for (size_t i = 0; i < loadManager_->GetPointLightInfo().size(); i++)
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

	if (currentScene_->GetIsSceneFinsh() == true) {
		oldSceneType_ = sceneType_;
		sceneChangeFlag = true;
	}

	if (sceneChangeFlag == false) {
		return;
	}

	// シーンが変わっていたら
	if (oldSceneType_ != sceneType_) {
		blackAlpha -= 0.025f;
		blackOut->SetColor({ 1,1,1,blackAlpha });
		if (blackAlpha <= 0) {
			blackAlpha = 0;
			sceneChangeFlag = false;
		}
	}
	// シーンが変わっていなかったら
	else {
		blackAlpha += 0.025f;
		blackOut->SetColor({ 1,1,1,blackAlpha });
		if (blackAlpha >= 1) {
			blackAlpha = 1;
			
			// 現在のシーンタイプごとに次のシーンをセット
			switch (sceneType_)
			{
			case SceneManager::Title:
				SetNextScene(Game);
				break;
			case SceneManager::Game:
				if (currentScene_->GetIsGameClear() == true) {
					SetNextScene(GameClear);
				}
				else {
					SetNextScene(GameOver);
				}
				break;
			case SceneManager::GameClear:
				SetNextScene(Title);
				break;
			case SceneManager::GameOver:
				SetNextScene(Title);
				break;
			default:
				break;
			}

		}
	}
}
