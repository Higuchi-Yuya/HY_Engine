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
	// ���͂̏�����
	input_ = std::make_unique<Input>();
	input_->Initialize();

#pragma region ���C�g�̏�����
	// ���C�g�̐���
	light.reset(LightGroup::Create());
	// ���C�g�ݒ�
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

	// 3D�I�u�W�F�N�g�Ƀ��C�g���Z�b�g
	Object3d::SetLight(light.get());
#pragma endregion

#pragma region �t�H�O
	fog.reset(Fog::Create());
	fog->nearFog = 10;
	fog->farFog = 100;
	fog->isActiveFog = false;
	Object3d::SetFog(fog.get());
#pragma endregion

#pragma region �e�N�X�`���ǂݍ���
	// �e�N�X�`���n���h���̓ǂݍ���

	// ���̃X�v���C�g�̃n���h��
	textureHandleDefu.reset(TextureManager::Load2DTextureP("texture.png"));
	
	// �V�[���`�F���W�Ŏg���n���h��
	blackOutTexHandle_.reset(TextureManager::Load2DTextureP("SceneChageTex.png"));

	// �^�C�g���̕����Ŏg���n���h���i���j
	titleFontTexHandle.reset(TextureManager::Load2DTextureP("kariTitle.png"));

	// �^�C�g���̔w�i�Ŏg���n���h���i���j
	titleBackTexHandle.reset(TextureManager::Load2DTextureP("titleBack.png"));

	// �^�C�g����A�{�^�������t�H���g�i���j
	titleButtonTexHandle.reset(TextureManager::Load2DTextureP("PressA.png"));


#pragma endregion

#pragma region ���f���̓ǂݍ���
	// �v���C���[�̃��f���ǂݍ���
	playerModel_.reset(Model::LoadFromOBJ("chr_sword", true));

	// �G�̃��f���ǂݍ���
	modelMedama_.reset(Model::LoadFromOBJ("Medama", true));

	// �w�i���f���֘A
	modelSkydome.reset(Model::LoadFromOBJ("skydome"));
	modelGround.reset(Model::LoadFromOBJ("ground"));
#pragma endregion

#pragma region �T�E���h�ǂݍ���
	sound.SoundLoadWave("GameClear.wav");
#pragma endregion

#pragma region �X�v���C�g������
	// �X�v���C�g�̏�����

	// ���̃X�v���C�g
	spriteProvisional = std::make_unique<Sprite>();
	spriteProvisional->Initialize(textureHandleDefu.get(), { 200,200 }, { 150,150 }, { 0.5f,0.5f, 0.5f,1.0f });
	spritePos = spriteProvisional->GetPosition();

	// �V�[���`�F���W�p�̃X�v���C�g
	blackOut = std::make_unique<Sprite>();
	blackOut->Initialize(blackOutTexHandle_.get(), { WinApp::window_width / 2,WinApp::window_height / 2 }, { 1280 * 2,720 * 2 });
	blackOut->SetColor({ 1,1,1,blackAlpha });

	// �^�C�g���̕����p�̃X�v���C�g
	titleKariFont = std::make_unique<Sprite>();
	titleKariFont->Initialize(titleFontTexHandle.get(), { WinApp::window_width / 2,WinApp::window_height / 2 - 150 },{500,100});

	// �^�C�g���̃{�^��A�̃X�v���C�g
	titleKariPressA = std::make_unique<Sprite>();
	titleKariPressA->Initialize(titleButtonTexHandle.get(), { WinApp::window_width / 2,WinApp::window_height / 2 + 200 },{250,50});

	// �^�C�g���̔w�i�̃X�v���C�g
	titleKariBack = std::make_unique<Sprite>();
	titleKariBack->Initialize(titleBackTexHandle.get(), { WinApp::window_width / 2,WinApp::window_height / 2 }, { 1280,720 });

#pragma endregion

#pragma region �v���C���[�֘A�̏�����
	player_ = std::make_unique<Player>();
	player_.reset(Player::Create(playerModel_.get()));
#pragma endregion

#pragma region ���[�_�[�p�̓ǂݍ���
	// ���x���f�[�^�̓ǂݍ���
	levelData_.reset(LevelLoader::LoadFile("testScene"));


	// �I�u�W�F�N�g�̏�����
	objSkydome.reset(Object3d::Create());
	objGround.reset(Object3d::Create());

	objSkydome->SetModel(modelSkydome.get());
	objGround->SetModel(modelGround.get());

	// ���f���f�[�^�����f���̃��X�g�ɓo�^
	models.insert(std::make_pair("skydome", modelSkydome.get()));
	models.insert(std::make_pair("ground", modelGround.get()));
	models.insert(std::make_pair("chr_sword", playerModel_.get()));
	models.insert(std::make_pair("sphere", modelMedama_.get()));

	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	//	�܂��A�v���C���[�̏����ʒu��G�l�~�[�̏���
	for (auto& objectData : levelData_->objects) {
		// �t�@�C��������o�^�ς݃��f��������
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

		// �^�O�����v���C���[�Ȃ�
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
		// �^�O�����G�l�~�[�Ȃ�
		else if (objectData.tagName == "enemy" && player_ != nullptr)
		{
			WorldTransform w;
			w.Initialize();
			w.translation = objectData.translation;
			w.scale = objectData.scaling;
			w.rotation = objectData.rotation;

			// �V�����G�̐���
			Enemy* newEnemy;
			newEnemy->Initialize(modelMedama_.get(), player_.get());

		}
		// ����ȊO�̃^�O���܂��͂Ȃ��̏ꍇ
		else 
		{
			// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
			Object3d* newObject = Object3d::Create();
			newObject->SetModel(model);

			// ���W
			Vector3 pos;
			pos = objectData.translation;
			newObject->worldTransform_.translation = pos;

			// ��]�p
			Vector3 rot;
			rot = objectData.rotation;
			newObject->worldTransform_.rotation = rot;

			// �X�P�[��
			Vector3 scale;
			scale = objectData.scaling;
			newObject->worldTransform_.scale = scale;

			// �z��ɓo�^
			objects.push_back(newObject);
		}
		
	}
#pragma endregion

#pragma region �G�l�~�[�֘A�̏�����
	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(modelMedama_.get(), player_.get());
#pragma endregion

#pragma region �r���[�v���W�F�N�V�����֘A�̏�����
	// �r���[�v���W�F�N�V�����̏�����
	gameCamera = std::make_unique<GameCamera>();
	gameCamera->Initialize(player_->GetCameraWorld());
	player_->SetGameCamera(gameCamera.get());
#pragma endregion

#pragma region �R���C�_�[�֘A�̏�����
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
	// �V�[���`�F���W����
	SceneChageUpdate();

	// �V�[���̂��ꂼ��̍X�V����
	switch (scene)
	{
	case GameScene::Scene::Title: // �^�C�g���V�[��
		TitleUpdate();
		break;
	case GameScene::Scene::Game: // �Q�[���V�[��
		GameSceneUpdate();
		break;
	case GameScene::Scene::Result: // ���U���g�V�[��

		break;
	default:
		break;
	}
	

}

void GameScene::ImguiUpdate()
{

	// �\�����ڂ̒ǉ�--------//
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

	// ���s����
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
	// �|�C���g���C�g
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
	// �X�|�b�g���C�g
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
	// �ۉe
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
	// �t�H�O
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

	// �t�H�O
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

	// �����蔻��-----------------------//
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
	case GameScene::Scene::Title: // �^�C�g���V�[��

		break;
	case GameScene::Scene::Game: // �Q�[���V�[��

		break;
	case GameScene::Scene::Result: // ���U���g�V�[��

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
	case GameScene::Scene::Title: // �^�C�g���V�[��

		break;
	case GameScene::Scene::Game: // �Q�[���V�[��

		// �I�u�W�F�N�g�֘A�̕`��
		objSkydome->Draw(&gameCamera->GetView());
		objGround->Draw(&gameCamera->GetView());

		player_->Draw(&gameCamera->GetView());

		enemy_->Draw(&gameCamera->GetView());

		objMedama_->Draw(&gameCamera->GetView());

		// FBX���f���̕`��
		FbxModel::PreDraw(commandList);


		FbxModel::PostDraw();

		// �p�[�e�B�N���̕`��
		DrawParticle();
		break;
	case GameScene::Scene::Result: // ���U���g�V�[��

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
	case GameScene::Scene::Title: // �^�C�g���V�[��
		titleKariBack->Draw();
		titleKariFont->Draw();
		titleKariPressA->Draw();
		
		break;
	case GameScene::Scene::Game: // �Q�[���V�[��
		spriteProvisional->Draw();
		break;
	case GameScene::Scene::Result: // ���U���g�V�[��

		break;
	default:
		break;
	}

	// �V�[���`�F���W�p�̂̃X�v���C�g�̕`��
	blackOut->Draw();
}

void GameScene::TitleUpdate()
{
	// �p�b�h��A�{�^�����������A�������̓X�y�[�X�L�[���������u��
	if (JoypadInput::GetButtonDown(PadCode::ButtonA)||input_->TriggerKey(DIK_SPACE)) {
		oldScene = Scene::Title;
		sceneChangeFlag = true;
	}
}

void GameScene::GameSceneUpdate()
{
	// ���͂̍X�V
	input_->Update();

	// ������0�L�[��������Ă�����
	if (input_->PushKey(DIK_0))
	{
		OutputDebugStringA("Hit 0\n");  // �o�̓E�B���h�E�ɁuHit 0�v�ƕ\��
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

	// �����蔻��֘A�̍X�V����
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
					// �����Ƀ��Z�b�g�֐���u��
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
			// �Q�[���V�[�����烊�U���g�V�[��
			else {
				blackAlpha += 0.025f;
				blackOut->SetColor({ 1,1,1,blackAlpha });
				if (blackAlpha >= 1) {
					blackAlpha = 1;
					scene = Scene::Result;
					// �����Ƀ��Z�b�g�֐���u��
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
						// �����Ƀ��Z�b�g�֐���u��
					}
					else if (resultChange == true) {
						scene = Scene::Title;
						// �����Ƀ��Z�b�g�֐���u��
					}
				}
			}
			break;
		default:
			break;
		}
	}
}
