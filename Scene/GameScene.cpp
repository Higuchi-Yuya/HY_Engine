#include "GameScene.h"
#include "Collision.h"
GameScene::~GameScene()
{
	// 入力解放
	delete input;
	// スプライトの解放
	delete sprite;
	delete sprite2;
	// オブジェクトの解放
	delete object3d;
	delete obj_2;
	delete objFighter;
	delete groundObj;
	// モデルの解放
	delete model;
	delete model_2;
	delete modelFighter;
	delete groundModel;
	// ビューの解放
	delete view;
	// ライトの解放
	delete light;
}

void GameScene::Initialize()
{
	// 入力の初期化
	input = new Input();
	input->Initialize();

	// ライトの生成
	light = LightGroup::Create();
	// ライト設定
	light->SetDirLightActive(0, true);
	light->SetDirLightActive(1, true);
	light->SetDirLightActive(2, true);
	//light->SetPointLightActive(0, true);
	//pointLightPos[0] = 0.5f;
	//pointLightPos[1] = 1.0f;
	//pointLightPos[2] = 0.0f;

	light->SetPointLightActive(0, false);
	light->SetPointLightActive(1, false);
	light->SetPointLightActive(2, false);
	//light->SetSpotLightActive(0, true);
	light->SetCircleShadowActive(0, true);

	// 3Dオブジェクトにライトをセット
	Object3d::SetLight(light);

	// テクスチャハンドルの読み込み
	textureHandle = Texture::LoadTexture("skydome/Nebura.jpg");
	textureHandle2 = Texture::LoadTexture("texture.png");

	// スプライトの初期化
	sprite = new Sprite();
	sprite2 = new Sprite();

	sprite->Initialize(textureHandle, { WinApp::window_width / 2,WinApp::window_height / 2 }, { 1280,720 });
	sprite2->Initialize(textureHandle2, { 200,200 });

	// モデルの読み込み
	model = Model::LoadFromOBJ("skydome", true);
	model_2 = Model::LoadFromOBJ("Medama", true);
	groundModel = Model::LoadFromOBJ("ground");
	modelFighter = Model::LoadFromOBJ("chr_sword");
	
	// オブジェクトの初期化
	object3d = Object3d::Create();
	obj_2 = Object3d::Create();
	objFighter = Object3d::Create();

	groundObj = Object3d::Create();


	objFighter->SetModel(modelFighter);
	objFighter->worldTransform_.position_ = fighterPos;


	object3d->SetModel(model);

	obj_2->SetModel(model_2);
	obj_2->worldTransform_.position_ = { 0,1,0 };
	obj_2->worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	obj_2->worldTransform_.color_ = { 1.0f,1.0f,1.0f,0.5f };

	groundObj->SetModel(groundModel);

	// ビュープロジェクションの初期化
	view = new ViewProjection;
	view->DebugCameraInitialze(input);
	view->target.y = 1.0f;
	view->SetDistance(3.0f);

	spritePos = sprite2->GetPosition();

	rotation = keisan.MakeAxisAngle({ 0.0f,0.0f,1.0f }, 3.141592 / 2.0f);
	pointY = { 0.0f,1.0f,0.0f };
	rotateMatrix = rotation.MakeRotateMatrix(rotation);
	rotateByQuaternion = keisan.RotateVector(pointY, rotation);
	rotateByMatrix = rotateMatrix.transform(pointY, rotateMatrix);

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

	//light->SetPointLightPos(0, Vector3(pointLightPos[0], pointLightPos[1], pointLightPos[2]));
	//light->SetPointLightColor(0, Vector3(pointLightColor[0], pointLightColor[1], pointLightColor[2]));
	//light->SetPointLightAtten(0, Vector3(pointLightAtten[0], pointLightAtten[1], pointLightAtten[2]));

	//light->SetSpotLightDir(0, spotLightDir);
	//light->SetSpotLightPos(0, spotLightPos);
	//light->SetSpotLightColor(0, spotLightColor);
	//light->SetSpotLightAtten(0, spotLightAtten);
	//light->SetSpotLightFactorAngle(0, spotLightFactorAngle);

	light->SetCircleShadowDir(0, circleShadowDir);
	light->SetCircleShadowCasterPos(0, fighterPos);
	light->SetCircleShadowAtten(0, circleShadowAtten);
	light->SetCircleShadowFactorAngle(0, circleShadowFactorAngle);

	objFighter->worldTransform_.position_ = fighterPos;

	light->Update();

	//object3d->SetScale(scale_);
	object3d->Update();

	objFighter->worldTransform_.rotation_.y += 0.01f;
	objFighter->Update();
	//spritePos = sprite2->GetPosition();
	sprite2->SetPosition(spritePos);
	
	obj_2->worldTransform_.rotation_.y += 0.01f;
	obj_2->Update();

	groundObj->Update();

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
	hit = Collision::CheckSphere2Triangle(sphere, triangle, &inter);
}

void GameScene::ImguiUpdate()
{

	// 表示項目の追加--------//
	ImGui::Begin("Sprite 1");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));
	
	ImGui::SliderFloat2("position", &spritePos.x, 0.0f, 1200.0f, "%.1f");



	ImGui::InputFloat3("rotateByQuaternion", &rotateByQuaternion.x);

	ImGui::InputFloat3("rotateByMatrix", &rotateByMatrix.x);


	ImGui::InputFloat3("sphere", &sphere.center.x);
	ImGui::Text("hit:%d", hit);
	ImGui::InputFloat3("sphere.inter", &inter.x);

	if (ImGui::Button("Reset")) {
		spritePos = { 200.0f,200.0f };
	}

	ImGui::End();

	ImGui::Begin("Light");

	//ImGui::SetWindowPos(ImVec2(100, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 200));
	
	ImGui::InputFloat3("circleShadowDir", &circleShadowDir.x);
	//ImGui::ColorEdit3("spotLightColor", &spotLightColor.x, ImGuiColorEditFlags_Float);
	ImGui::InputFloat3("circleShadowAtten", &circleShadowAtten.x);
	ImGui::InputFloat2("circleShadowFactorAngle", &circleShadowFactorAngle.x);
	ImGui::InputFloat3("fighterPos", &fighterPos.x);

	ImGui::End();
	// ---------------------//

}

void GameScene::Draw2DBack()
{
	//sprite->Draw();
}

void GameScene::Draw3D()
{
	object3d->Draw(view);
	

	obj_2->Draw(view);
	objFighter->Draw(view);

	groundObj->Draw(view);
}

void GameScene::Draw2DFront()
{

	sprite2->Draw();
}
