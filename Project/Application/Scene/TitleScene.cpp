#include "TitleScene.h"
#include "InputManager.h"

TitleScene::~TitleScene()
{
	gameCamera_ = nullptr;
	lightGroup_ = nullptr;
	pointLightsInfo_.clear();
	objects.clear();
	latticeDoors_.clear();
	ranterns_.clear();
	highLumiRanterns_.clear();
}

void TitleScene::Initialize()
{
	// タイトルの文字で使うハンドル（仮）
	titleFontTexHandle.reset(TextureManager::Load2DTextureP("kariTitle.dds"));

	// タイトルのAボタン押すフォント（仮）
	titleButtonTexHandle.reset(TextureManager::Load2DTextureP("PressA.png"));

	// タイトルの文字用のスプライト
	titleKariFont = std::make_unique<Sprite>();
	titleKariFont->Initialize(titleFontTexHandle.get(), { WinApp::window_width / 2,WinApp::window_height / 2 - 150 }, { 500,100 });

	// タイトルのボタンAのスプライト
	titleKariPressA = std::make_unique<Sprite>();
	titleKariPressA->Initialize(titleButtonTexHandle.get(), { WinApp::window_width / 2,WinApp::window_height / 2 + 200 }, { 250,50 });

	testModel1_.reset(Model::LoadFromOBJ("box1x1x1"));
	testModel2_.reset(Model::LoadFromOBJ("ground"));
	testModel3_.reset(Model::LoadFromOBJ("skydome"));

	testObj1_.reset(DeferredObject3d::Create());
	testObj1_->SetModel(testModel1_.get());
	testObj1_->worldTransform_.translation.y += 3;
	testObj1_->Update();
	testObj2_.reset(DeferredObject3d::Create());
	testObj2_->SetModel(testModel2_.get());
	testObj3_.reset(DeferredObject3d::Create());
	testObj3_->SetModel(testModel3_.get());
}

void TitleScene::Update()
{
	lightGroup_->SetSpotLightActive(0, false);
	lightGroup_->SetCircleShadowActive(0, false);
	lightGroup_->SetPointLightActive((int)(pointLightsInfo_.size() + 1), false);

	// パッドでAボタンを押すか、もしくはスペースキーを押した瞬間
	if (JoypadInput::GetButtonDown(PadCode::ButtonA)) {
		gameCamera_->SetIsCanEase(true);
		IsTitleAlpha_ = true;
	}
	// スプライトのアルファを下げる処理
	if (IsTitleAlpha_ == true) {
		titleAlpha_ -= 0.05f;
		titleKariFont->SetColor(Vector4(1, 1, 1, titleAlpha_));
		titleKariPressA->SetColor(Vector4(1, 1, 1, titleAlpha_));
	}
	else {
		titleKariFont->SetColor(Vector4(1, 1, 1, 1));
		titleKariPressA->SetColor(Vector4(1, 1, 1, 1));
	}

	// オブジェクトの更新処理
	for (auto o : objects) {
		o->Update();
	}

	// オブジェクトの更新処理
	for (auto L : ranterns_) {
		L->Update();
	}

	// お墓のドアのオブジェクトの描画
	if (gameCamera_->GetIsCanEase() == true) {
		latticeDoors_[0]->worldTransform_.rotation.y -= MathUtil::DegreeToRadian(2);
		latticeDoors_[1]->worldTransform_.rotation.y += MathUtil::DegreeToRadian(2);
	}

	if (gameCamera_->GetIsEaseEnd() == true) {
		IsSceneFinsh_ = true;
		latticeDoors_[0]->worldTransform_.rotation.y = MathUtil::DegreeToRadian(-90 - 75);
		latticeDoors_[1]->worldTransform_.rotation.y = MathUtil::DegreeToRadian(90 + 75);
	}

	for (auto l : latticeDoors_)
	{
		l->Update();
	}

	// カメラ更新処理
	gameCamera_->TitleUpdate();
}

void TitleScene::ImguiUpdate()
{
}

bool TitleScene::GetIsGameClear()
{
	return false;
}

void TitleScene::Draw2DBack()
{
}

void TitleScene::DrawForward3D()
{
}

void TitleScene::DrawDefrred3D()
{
	// お墓のドアのオブジェクトの描画
	for (auto l : latticeDoors_) {

		l->Draw(&gameCamera_->GetView());
	}

	// 普通のオブジェクトの描画
	for (auto o : objects) {

		o->Draw(&gameCamera_->GetView());
	}
	// ランタンのオブジェクトの描画
	for (auto L : ranterns_) {
		L->Draw(&gameCamera_->GetView());
	}
	//testObj1_->Draw(&gameCamera_->GetView());
	//testObj2_->Draw(&gameCamera_->GetView());
	//testObj3_->Draw(&gameCamera_->GetView());
}

void TitleScene::DrawParticle()
{
}

void TitleScene::Draw2DFront()
{
	titleKariFont->Draw();
	titleKariPressA->Draw();
}

void TitleScene::DrawBloomObject()
{
	// ランタンのオブジェクトの描画
	for (auto L : ranterns_) {
		L->Draw(&gameCamera_->GetView());
	}
}

void TitleScene::DrawHighLumiObj()
{
	Object3d::SetBlendMode(Object3d::BlendMode::BLOOM);
	// ランタンのオブジェクトの描画
	for (auto L : highLumiRanterns_) {
		L->Update();
		L->Draw(&gameCamera_->GetView());
	}
	Object3d::SetBlendMode(Object3d::BlendMode::NORMAL);
}

void TitleScene::Reset()
{
	titleKariFont->SetColor(Vector4(1, 1, 1, 1));
	titleKariPressA->SetColor(Vector4(1, 1, 1, 1));
	IsSceneFinsh_ = false;
	IsTitleAlpha_ = false;
	titleAlpha_ = 1;
}

void TitleScene::SetObjs(std::vector<DeferredObject3d*> objs,ObjsType objType)
{
	switch (objType)
	{
	case ObjsType::Normal:
		objects = objs;
		break;
	case ObjsType::Doors:
		latticeDoors_ = objs;
		break;
	case ObjsType::Ranterns:
		ranterns_ = objs;
		break;
	default:
		break;
	}
}

void TitleScene::SetBloomObjs(std::vector<Object3d*> objs)
{
	highLumiRanterns_ = objs;
}
