#include "GameScene.h"
#include "Collision.h"
#include "SphereCollider.h"
#include "CollisionManager.h"
#include "Player.h"
#include "MeshCollider.h"
#include "TouchableObject.h"
#include <time.h>

#include "FbxLoader.h"

GameScene::~GameScene()
{
	// 入力解放
	delete input;
	// スプライトの解放
	//delete sprite;
	delete sprite2;
	// オブジェクトの解放
	delete object3d;

	delete objMedama;


	delete objFighter;
	delete groundObj;
	//delete groundObj2;
	//for (auto object : objects) {
	//	delete object;
	//}
	//objects.clear();
	delete point1;
	delete point2;
	delete point3;
	delete rayobj;

	// モデルの解放
	delete model;
	delete model_2;
	delete modelFighter;
	delete groundModel;
	delete modelPlane;
	delete modelBox;
	delete modelPyramid;

	// ビューの解放
	delete view;
	// ライトの解放
	delete light;
	// フォグの解放
	delete fog;

	delete atariModel;
	delete atariObj;
}

void GameScene::Initialize()
{
	srand((unsigned int)time(NULL));
	// 入力の初期化
	input = new Input();
	input->Initialize();

	// ライトの生成
	light = LightGroup::Create();
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
	Object3d::SetLight(light);

	fog = Fog::Create();
	fog->nearFog = 10;
	fog->farFog = 100;
	fog->isActiveFog = false;
	Object3d::SetFog(fog);

	// テクスチャハンドルの読み込み
	//textureHandle = Texture::LoadTexture("skydome/Nebura.jpg");
	textureHandle2 = Texture::Load2DTexture("texture.png");

	// スプライトの初期化
	//sprite = new Sprite();
	sprite2 = new Sprite();

	//sprite->Initialize(textureHandle, { WinApp::window_width / 2,WinApp::window_height / 2 }, { 1280,720 });
	sprite2->Initialize(textureHandle2, { 200,200 });

	// モデルの読み込み
	model = Model::LoadFromOBJ("skydome", true);
	model_2 = Model::LoadFromOBJ("doragon", true);
	groundModel = Model::LoadFromOBJ("ground2");
	modelFighter = Model::LoadFromOBJ("sphere");
	modelPlane = Model::LoadFromOBJ("plane1x1");
	modelBox = Model::LoadFromOBJ("box1x1x1");
	modelPyramid = Model::LoadFromOBJ("pyramid1x1");

	objFighter = Player::Create(modelFighter);
	objFighter->worldTransform_.position_ = { 0,5,0 };

	// モデルテーブル
	Model* modeltable[10] = {
		modelPlane,
		modelPlane,
		modelPlane,
		modelPlane,
		modelPlane,
		modelPlane,
		modelPlane,
		modelPlane,
		modelBox,
		modelPyramid,
	};

	const int DIV_NUM = 10;
	const float LAND_SCALE = 2.0f;
	//for (int i = 0; i < DIV_NUM; i++) {
	//	for (int j = 0; j < DIV_NUM; j++) {

	//		int modelIndex = rand() % 10;

	//		TouchableObject* object = TouchableObject::Create(modeltable[modelIndex]);
	//		object->worldTransform_.scale_ = { LAND_SCALE,LAND_SCALE,LAND_SCALE };
	//		object->worldTransform_.position_={ (j - DIV_NUM / 2) * LAND_SCALE, 0, (i - DIV_NUM / 2) * LAND_SCALE };
	//		objects.push_back(object);
	//	}
	//}

	// オブジェクトの初期化
	object3d = Object3d::Create();


	objMedama = Object3d::Create();
	objMedama->SetModel(model_2);
	objMedama->worldTransform_.position_ = { -1,1,0 };
	objMedama->worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	objMedama->worldTransform_.color_ = { 1.0f,1.0f,1.0f,1.0f };
	objMedama->SetCollider(new SphereCollider);

	collisionManager = CollisionManager::GetInstance();



	point1 = Object3d::Create();
	point2 = Object3d::Create();
	point3 = Object3d::Create();
	rayobj = Object3d::Create();

	
	groundObj = TouchableObject::Create(groundModel);
	//groundObj->worldTransform_.scale_.x += 5;

	//groundObj2 = TouchableObject::Create(modelBox);
	//groundObj2->worldTransform_.position_.x += 2;
	//groundObj2->UpdateWorldMatrix();


	objFighter->SetModel(modelFighter);
	objFighter->worldTransform_.position_ = fighterPos;


	object3d->SetModel(model);



	//groundObj->SetModel(groundModel);

	// ビュープロジェクションの初期化
	view = new ViewProjection;
	view->DebugCameraInitialze(input);
	view->target.y = 1.0f;
	view->SetDistance(3.0f);

	spritePos = sprite2->GetPosition();

	rotation0 = keisan.MakeAxisAngle({ 0.71f,0.71f,0.0f }, 0.3f);
	rotation1 = { -rotation0.x,-rotation0.y, -rotation0.z, -rotation0.w };//keisan.MakeAxisAngle({ 0.71f,0.0f,0.71f }, 3.141592f);

	interpolate0 = keisan.Slerp(rotation0, rotation1, 0.0f);
	interpolate1 = keisan.Slerp(rotation0, rotation1, 0.3f);
	interpolate2 = keisan.Slerp(rotation0, rotation1, 0.5f);
	interpolate3 = keisan.Slerp(rotation0, rotation1, 0.7f);
	interpolate4 = keisan.Slerp(rotation0, rotation1, 1.0f);

	dirToDir = keisan.DirectionToDirection(direction1, direction2);

	// 球の初期値を設定
	sphere.center = { 0,2,0 };
	sphere.radius = 1.0f;

	// 平面の初期値を設定
	plane.normal = { 0,1,0 };
	plane.distance = 0.0f;

	// 三角形の初期値を設定
	triangle.p0 = { -1.0f,0,-1.0f };
	triangle.p1 = { -1.0f,0,+1.0f };
	triangle.p2 = { +1.0f,0,-1.0f };

	triangle.normal = { 0.0f,1.0f,0.0f };

	// レイの初期値を設定
	ray.start = { 10,0.5f,0 };
	ray.dir = { 0,-1,0 };

	// 確認用オブジェ
	point1->SetModel(model_2);
	point2->SetModel(model_2);
	point3->SetModel(model_2);

	point1->worldTransform_.scale_ = { 0.2f,0.2f,0.2f };
	point1->worldTransform_.position_ = triangle.p0;
	point2->worldTransform_.scale_ = { 0.2f,0.2f,0.2f };
	point2->worldTransform_.position_ = triangle.p1;
	point3->worldTransform_.scale_ = { 0.2f,0.2f,0.2f };
	point3->worldTransform_.position_ = triangle.p2;

	rayobj->SetModel(modelFighter);
	rayobj->worldTransform_.scale_ = { 0.2f,1.0f,0.2f };

	sound.SoundLoadWave("GameClear.wav");

	atariModel = Model::LoadFromOBJ("sphere", true);
	atariObj = Object3d::Create();
	atariObj->SetModel(atariModel);

	// モデル名を指定してファイルを読み込み
	//FbxLoader::GetInstance()->LoadModelFromFile("cube");
}

void GameScene::Update()
{
	// 入力の更新
	input->Update();

	// 数字の0キーが押されていたら
	if (input->PushKey(DIK_0))
	{
		OutputDebugStringA("Hit 0\n");  // 出力ウィンドウに「Hit 0」と表示
	}
	if (input->TriggerKey(DIK_R)) {
		objFighter->worldTransform_.position_ = { 1.0f, 10.0f,0.0f };
		objFighter->UpdateWorldMatrix();
	}
	fighterPos = objFighter->worldTransform_.position_;



	light->SetPointLightPos(0, Vector3(pointLightPos[0], pointLightPos[1], pointLightPos[2]));
	light->SetPointLightColor(0, Vector3(pointLightColor[0], pointLightColor[1], pointLightColor[2]));
	light->SetPointLightAtten(0, Vector3(pointLightAtten[0], pointLightAtten[1], pointLightAtten[2]));

	light->SetSpotLightDir(0, spotLightDir);
	light->SetSpotLightPos(0, spotLightPos);
	light->SetSpotLightColor(0, spotLightColor);
	light->SetSpotLightAtten(0, spotLightAtten);
	light->SetSpotLightFactorAngle(0, spotLightFactorAngle);

	light->SetCircleShadowDir(0, circleShadowDir);
	light->SetCircleShadowCasterPos(0, fighterPos);
	light->SetCircleShadowAtten(0, circleShadowAtten);
	light->SetCircleShadowFactorAngle(0, circleShadowFactorAngle);

	light->SetAmbientColor(AmColor);
	light->SetDiffuseColor(DiColor);
	light->SetSpecularColor(SpColor);

	//objFighter->worldTransform_.position_ = fighterPos;

	light->Update();

	//object3d->SetScale(scale_);
	object3d->Update();



	//objFighter->worldTransform_.rotation_.y += 0.01f;
	collisionManager->CheckAllCollisions();


	objFighter->Update();
	//spritePos = sprite2->GetPosition();
	sprite2->SetPosition(spritePos);


	objMedama->worldTransform_.rotation_.y += 0.01f;
	objMedama->Update();


	//for (auto object : objects) {
	//	object->Update();
	//}
	groundObj->Update();
	//groundObj2->Update();
	view->DebugCameraUpdate();

	// 球移動
	{
		Vector3 moveY = { 0,0.01f,0 };
		if (input->PushKey(DIK_8)) { sphere.center += moveY; }
		else if (input->PushKey(DIK_2)) { sphere.center -= moveY; }

		Vector3 moveX = { 0.01f,0,0 };
		if (input->PushKey(DIK_6)) { sphere.center += moveX; }
		else if (input->PushKey(DIK_4)) { sphere.center -= moveX; }

	}
	// レイ操作
	{
		Vector3 moveZ = { 0,0,0.01f };
		if (input->PushKey(DIK_UP)) { ray.start += moveZ; }
		else if (input->PushKey(DIK_DOWN)) { ray.start -= moveZ; }

		Vector3 moveX = { 0.01f,0,0 };
		if (input->PushKey(DIK_RIGHT)) { ray.start += moveX; }
		else if (input->PushKey(DIK_LEFT)) { ray.start -= moveX; }
	}

	point1->Update();
	point2->Update();
	point3->Update();

	rayobj->worldTransform_.position_ = ray.start;
	rayobj->Update();


	hit = Collision::CheckRay2Sphere(ray, sphere, &distance, &inter);
	hitRay = Collision::CheckRay2Triangle(ray, triangle, &distance, &inter);

	fog->UpdateMatrix();
	Object3d::SetFog(fog);

	if (isActiveSound == true) {
		sound.SoundPlayWave(true, 0.01f);
		isActiveSound = false;
	}
	if (isStopSound == true) {
		sound.StopWave();
		isStopSound = false;
	}

	RaycastHit raycastHit;
	if (collisionManager->Raycast(ray, &raycastHit)) {
		atariObj->worldTransform_.position_ = raycastHit.inter;
		atariObj->Update();
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

	ImGui::InputFloat3("fighterPos", &fighterPos.x);

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
		ImGui::InputFloat3("circleShadowDir", &circleShadowDir.x);
		//ImGui::ColorEdit3("spotLightColor", &spotLightColor.x, ImGuiColorEditFlags_Float);
		ImGui::InputFloat3("circleShadowAtten", &circleShadowAtten.x);
		ImGui::InputFloat2("circleShadowFactorAngle", &circleShadowFactorAngle.x);
		ImGui::InputFloat3("fighterPos", &fighterPos.x);

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
	//sprite->Draw();
}

void GameScene::Draw3D()
{

	//object3d->Draw(view);


	objMedama->Draw(view);
	

	objFighter->Draw(view);

	//point1->Draw(view);
	//point2->Draw(view);
	//point3->Draw(view);
	//rayobj->Draw(view);
	//atariObj->Draw(view);
	
	//groundObj->Draw(view);
}

void GameScene::Draw2DFront()
{

	sprite2->Draw();
}
