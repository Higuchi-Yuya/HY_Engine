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

	Enemy::StaticFinalize();
}

void GameScene::Initialize()
{
	Random::Init();
	// ���͂̏�����
	input_ = std::make_unique<Input>();
	input_->Initialize();
	Enemy::StaticInitialize();
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
	modelSkydome_.reset(Model::LoadFromOBJ("skydome"));
	modelGround_.reset(Model::LoadFromOBJ("ground"));

	// ��
	modelTreeBald_.reset(Model::LoadFromOBJ("tree_bald"));
	modelTreeNormal_.reset(Model::LoadFromOBJ("tree_Normal"));

	// �t�F���X
	modelFence_.reset(Model::LoadFromOBJ("fence"));
	modelFencePost_.reset(Model::LoadFromOBJ("fencePost"));

	// ����
	modelGraveCross.reset(Model::LoadFromOBJ("grave_cross"));
	modelGraveSquare.reset(Model::LoadFromOBJ("grave_square"));

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

#pragma region �R���C�_�[�{�̂̏�����
	gameCollider = std::make_unique<GameCollider>();
	gameCollider->Initialize();
#pragma endregion

#pragma region ���[�_�[�p�̓ǂݍ���
	// ���x���f�[�^�̓ǂݍ���
	levelData_.reset(LevelLoader::LoadFile("field2"));

	// ���f���f�[�^�����f���̃��X�g�ɓo�^
	models.insert(std::make_pair("skydome", modelSkydome_.get()));
	models.insert(std::make_pair("ground", modelGround_.get()));
	models.insert(std::make_pair("chr_sword", playerModel_.get()));
	models.insert(std::make_pair("Medama", modelMedama_.get()));

	models.insert(std::make_pair("tree_bald", modelTreeBald_.get()));
	models.insert(std::make_pair("tree_Normal", modelTreeNormal_.get()));
	models.insert(std::make_pair("fence", modelFence_.get()));
	models.insert(std::make_pair("fencePost", modelFencePost_.get()));
	models.insert(std::make_pair("grave_cross", modelGraveCross.get()));
	models.insert(std::make_pair("grave_square", modelGraveSquare.get()));

	// ���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	//	�܂��A�v���C���[�̏����ʒu��G�l�~�[�̏���
	for (auto& objectData : levelData_->objects) {
		// �t�@�C��������o�^�ς݃��f��������
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

		// �^�O�����G�l�~�[�Ȃ�
		#pragma region �G�l�~�[�֘A�̏�����
		if (objectData.tagName == "enemy")
		{
			WorldTransform w;
			w.Initialize();
			w.translation = objectData.translation;
			w.scale = objectData.scaling;
			w.rotation = objectData.rotation;

			// �V�����G�̐���
			Enemy* newEnemy = new Enemy;
			newEnemy->SetWorldTransInfo(w);
			newEnemy->Initialize(modelMedama_.get(), player_.get());
			newEnemy->UpdateWorldMatrix();

			// ���쐬�����G��z��Ɋi�[
			enemys_.push_back(newEnemy);
		}
#pragma endregion

		// ����ȊO�̃^�O���܂��͂Ȃ��̏ꍇ
		// ���ʂ̃I�u�W�F�N�g�Ɣ��f������
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

			// �^�O������������͖؂�������A�����蔻�������
			if (objectData.tagName == "tree"|| 
				objectData.tagName == "grave") {
				gameCollider->AddObj(newObject);
			}
		}
		
	}
#pragma endregion

#pragma region �r���[�v���W�F�N�V�����֘A�̏�����
	// �r���[�v���W�F�N�V�����̏�����
	gameCamera = std::make_unique<GameCamera>();
	gameCamera->Initialize(player_->GetCameraWorld());
	gameCamera->SetCameraFPos(player_->worldTransform_.translation);
	player_->SetGameCamera(gameCamera.get());
#pragma endregion

#pragma region �R���C�_�[�֘A�̏�����


	for (auto e:enemys_){
		gameCollider->AddEnemy(e);
	}
	
	gameCollider->SetPlayer(player_.get());
#pragma endregion

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
		ResultSceneUpdate();
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

		// �v���C���[�̕`��
		player_->Draw(&gameCamera->GetView());

		// �G�̕`��
		for (auto e : enemys_) {
			e->Draw(&gameCamera->GetView());
		}

		// ���ʂ̃I�u�W�F�N�g�̕`��
		for (auto o : objects) {
			o->Draw(&gameCamera->GetView());
		}
		gameCollider->Draw3D(&gameCamera->GetView());
		// FBX���f���̕`��
		FbxModel::PreDraw(commandList);


		FbxModel::PostDraw();

		ParticleManager::PreDraw(commandList);
		// �p�[�e�B�N���̕`��
		DrawParticle();
		// �G�̕`��
		for (auto e : enemys_) {
			e->DrawParticle(&gameCamera->GetView());
		}
		ParticleManager::PostDraw();
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
		//spriteProvisional->Draw();
		player_->Draw2DFront();
		break;
	case GameScene::Scene::Result: // ���U���g�V�[��

		break;
	default:
		break;
	}

	// �V�[���`�F���W�p�̂̃X�v���C�g�̕`��
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

		// �E�F�[�u�̃��Z�b�g
		waveTimeNum_ = 0;
		waveTimer_ = 0;

		// �Q�[���R���C�_�[�̃��Z�b�g
		gameCollider->Reset();

		//	�G�l�~�[��S�폜
		for (auto e : enemys_) {
			delete e;
		}
		enemys_.clear();

		// �G�l�~�[�̍ēǂݍ���
		LoadEnemy();

		// �G�l�~�[�̃X�|�[������Ԋu�����Z�b�g
		enemySpawnTimer_ = 0;

		// �v���C���[�̃��Z�b�g
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
	case GameScene::wave01:// �E�F�[�u�O�P�̎��̓G�̃X�|�[���p�^�[�����I

		randomWave01_ = Random::Range(1, 3);
		
		switch (randomWave01_)
		{
		case 1:
			// ���x���f�[�^�̓ǂݍ���
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave01_p1"));
			break;
		case 2:
			// ���x���f�[�^�̓ǂݍ���
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave01_p2"));
			break;
		case 3:
			// ���x���f�[�^�̓ǂݍ���
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave01_p3"));
			break;
		default:
			break;
		}

		break;
	case GameScene::wave02:// �E�F�[�u�O�Q�̎��̓G�̃X�|�[���p�^�[�����I
		randomWave01_ = Random::Range(1, 3);

		switch (randomWave01_)
		{
		case 1:
			// ���x���f�[�^�̓ǂݍ���
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave02_p1"));
			break;
		case 2:
			// ���x���f�[�^�̓ǂݍ���
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave02_p2"));
			break;
		case 3:
			// ���x���f�[�^�̓ǂݍ���
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave02_p3"));
			break;
		default:
			break;
		}

		break;
	case GameScene::wave03:// �E�F�[�u�O�R�̎��̓G�̃X�|�[���p�^�[�����I

		randomWave01_ = Random::Range(1, 3);

		switch (randomWave01_)
		{
		case 1:
			// ���x���f�[�^�̓ǂݍ���
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave03_p1"));
			break;
		case 2:
			// ���x���f�[�^�̓ǂݍ���
			levelData_.reset(LevelLoader::LoadFile("Enemy/enemyDataWave03_p2"));
			break;
		case 3:
			// ���x���f�[�^�̓ǂݍ���
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
		// �t�@�C��������o�^�ς݃��f��������
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

		// �^�O�����G�l�~�[�Ȃ�
#pragma region �G�l�~�[�֘A�̏�����
		if (objectData.tagName == "enemy")
		{
			WorldTransform w;
			w.Initialize();
			w.translation = objectData.translation;
			w.scale = objectData.scaling;
			w.rotation = objectData.rotation;

			// �V�����G�̐���
			Enemy* newEnemy = new Enemy;
			newEnemy->SetWorldTransInfo(w);
			newEnemy->Initialize(modelMedama_.get(), player_.get());
			newEnemy->UpdateWorldMatrix();

			// ���쐬�����G��z��Ɋi�[
			enemys_.push_back(newEnemy);
			// ���쐬�����G�𓖂��蔻��ɏ���o�^
			gameCollider->AddEnemy(newEnemy);
		}
#pragma endregion
	}
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

#pragma region ���C�g�̍X�V����
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

#pragma region �v���C���[�̍X�V����
	// �v���C���[�̍X�V����
	player_->Update();

	// �v���C���[�����񂾂�Ƃ肠�������U���g��ʂɈڍs
	if (player_->GetIsAlive() == false) {
		oldScene = Scene::Game;
		sceneChangeFlag = true;
	}
#pragma endregion


#pragma region �G�̍X�V�����֘A
	
	EnemyGameUpdate();
#pragma endregion

	
	// �J�����̍X�V����
	gameCamera->SetCameraPos(player_->worldTransform_.translation);
	gameCamera->Update();

	// �I�u�W�F�N�g�̍X�V����
	for (auto o : objects) {
		o->Update();
	}

	// �����蔻��֘A�̍X�V����
	gameCollider->Updata();

	// ���C�g�̍X�V����
	circleShadowCasterPos = player_->GetWorldPosition();
	light->SetCircleShadowCasterPos(0, circleShadowCasterPos);
	light->Update();


	//// �G���S�ł�����Ƃ肠�����V�[����؂�ւ���
	//if (enemys_.size() <= 1) {
	//	oldScene = Scene::Game;
	//	sceneChangeFlag = true;
	//}
}

void GameScene::ResultSceneUpdate()
{
	// �p�b�h��A�{�^�����������A�������̓X�y�[�X�L�[���������u��
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
						// �����Ƀ��Z�b�g�֐���u��
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

	// �G�l�~�[�̃E�F�[�u�̎��Ԃ�i�߂�
	waveTimer_++;

	// �G�l�~�[�̃E�F�[�u�̎��Ԃ��}�b�N�X�ɂȂ�����E�F�[�u��i�߂�
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

		// �E�F�[�u�̎��Ԃ����Z�b�g
		waveTimer_ = 0;
	}

	// �G�l�~�[�̎��Ԃ��Ƃɂ킭���� (��������)
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

	//�������s�����G��S�폜
	auto it = std::partition(enemys_.begin(), enemys_.end(), [](Enemy* a)
		{return a->GetDeadMotionEnd() == true; });
	std::for_each(enemys_.begin(), it, [](Enemy* a) { delete a; });
	enemys_.erase(enemys_.begin(), it);

	//	�G�̍X�V����
	for (auto e : enemys_) {
		e->Update();
	}
}
