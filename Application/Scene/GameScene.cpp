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

	fog.reset(Fog::Create());
	fog->nearFog = 10;
	fog->farFog = 100;
	fog->isActiveFog = false;
	Object3d::SetFog(fog.get());

	// �e�N�X�`���n���h���̓ǂݍ���

	textureHandleDefu.reset(TextureManager::Load2DTextureP("texture.png"));

	// �X�v���C�g�̏�����
	spriteProvisional = std::make_unique<Sprite>();

	
	spriteProvisional->Initialize(textureHandleDefu.get(), {200,200}, {150,150}, {0.5f,0.5f, 0.5f,1.0f});

	// ���f���̓ǂݍ���
	playerModel_.reset(Model::LoadFromOBJ("chr_sword", true));
	modelMedama_.reset(Model::LoadFromOBJ("Medama", true));


	const int DIV_NUM = 10;
	const float LAND_SCALE = 2.0f;

	// �I�u�W�F�N�g�̏�����
	spritePos = spriteProvisional->GetPosition();
	sound.SoundLoadWave("GameClear.wav");



#pragma region ���[�_�[�p�̓ǂݍ���
	//// ���x���f�[�^�̓ǂݍ���
	//levelData = LevelLoader::LoadFile("testScene");

	// ���f���ǂݍ���
	modelSkydome.reset(Model::LoadFromOBJ("skydome"));
	modelGround.reset(Model::LoadFromOBJ("ground"));

	objSkydome.reset(Object3d::Create());
	objGround.reset(Object3d::Create());

	objSkydome->SetModel(modelSkydome.get());
	objGround->SetModel(modelGround.get());

	//modelFighter = Model::LoadFromOBJ("chr_sword", true);
	//modelSphere = Model::LoadFromOBJ("sphere", true);

	//models.insert(std::make_pair("skydome", modelSkydome));
	//models.insert(std::make_pair("ground", modelGround));
	//models.insert(std::make_pair("chr_sword", modelFighter));
	//models.insert(std::make_pair("sphere", modelSphere));

	//���x���f�[�^����I�u�W�F�N�g�𐶐��A�z�u
	//for (auto& objectData : levelData->objects) {
	//	// �t�@�C��������o�^�ς݃��f��������
	//	Model* model = nullptr;
	//	decltype(models)::iterator it = models.find(objectData.fileName);
	//	if (it != models.end()) {
	//		model = it->second;
	//	}

	//	// ���f�����w�肵��3D�I�u�W�F�N�g�𐶐�
	//	Object3d* newObject = Object3d::Create();
	//	newObject->SetModel(model);

	//	// ���W
	//	Vector3 pos;
	//	pos = objectData.translation;
	//	newObject->worldTransform_.position_ = pos;

	//	// ��]�p
	//	Vector3 rot;
	//	rot= objectData.rotation;
	//	newObject->worldTransform_.rotation_ = rot;

	//	// �X�P�[��
	//	Vector3 scale;
	//	scale= objectData.scaling;
	//	newObject->worldTransform_.scale_ = scale;

	//	// �z��ɓo�^
	//	objects.push_back(newObject);
	//}
#pragma endregion

	

	// ���f�������w�肵�ăt�@�C����ǂݍ���
	//FbxLoader::GetInstance()->LoadModelFromFile("cube");
	//fbxmodel_.reset(FbxLoader::GetInstance()->LoadModelFromFile("boneTest"));
	//fbxmodel_->Initialize();
	//fbxTrans_.Initialize();
	//fbxTrans_.scale = { 0.01f,0.01f, 0.01f };
	//fbxTrans_.UpdateMatrix();

	////modelAnim_ = std::make_unique<FbxAnimetion>();
	////modelAnim_->Load("boneTest");
	//

	player_ = std::make_unique<Player>();

	player_.reset(Player::Create(playerModel_.get()));

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize(modelMedama_.get(), player_.get());

	objMedama_.reset(Object3d::Create());
	objMedama_->SetModel(modelMedama_.get());
	objMedama_->worldTransform_.translation = { 2,1,0 };

	objMedama_->dissolve_.isActiveDissolve_ = true;


	objMedama_->Update();

	// �r���[�v���W�F�N�V�����̏�����
	gameCamera = std::make_unique<GameCamera>();
	gameCamera->Initialize(&player_->worldTransform_);

	gameCollider = std::make_unique<GameCollider>();
	gameCollider->Initialize();
	gameCollider->AddEnemy(enemy_.get());
	gameCollider->SetPlayer(player_.get());



}

void GameScene::Update()
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
	light->SetCircleShadowCasterPos(0, player_->GetWorldPosition());
	light->SetCircleShadowAtten(0, circleShadowAtten);
	light->SetCircleShadowFactorAngle(0, circleShadowFactorAngle);

	light->SetAmbientColor(AmColor);
	light->SetDiffuseColor(DiColor);
	light->SetSpecularColor(SpColor);

	//objFighter->worldTransform_.position_ = fighterPos;

	light->Update();

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

}

void GameScene::Draw3D()
{
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	objSkydome->Draw(&gameCamera->GetView());
	objGround->Draw(&gameCamera->GetView());

	player_->Draw(&gameCamera->GetView());

	enemy_->Draw(&gameCamera->GetView());

	objMedama_->Draw(&gameCamera->GetView());

	// FBX���f���̕`��
	FbxModel::PreDraw(commandList);

	//fbxmodel_->Draw(&fbxTrans_, &gameCamera->GetView());

	FbxModel::PostDraw();

	DrawParticle();
}

void GameScene::DrawParticle()
{
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	gameCollider->Draw(commandList, &gameCamera->GetView());
}

void GameScene::Draw2DFront()
{

	spriteProvisional->Draw();
}
