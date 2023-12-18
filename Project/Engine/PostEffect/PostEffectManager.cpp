#include "PostEffectManager.h"

void PostEffectManager::Initialize()
{
	higuLumiTarget_ = std::make_unique<HiguLumiTarget>();
	higuLumiTarget_->Initialize();

	postLumi_ = std::make_unique<PostEffectHighLumi>();
	postLumi_->Initialize();

	gaussianBlur_ = std::make_unique<GaussianBlur>();
	gaussianBlur_->Initialize();

	postTarget_= std::make_unique<PostTarget>();
	postTarget_->Initialize();

	vignette_ = std::make_unique<Vignette>();
	vignette_->Initialize();

	beatEffect_ = std::make_unique<BeatEffect>();
	beatEffect_->Initialize();
	BeatEffect::SetGaussianBlur(gaussianBlur_.get());
}

void PostEffectManager::Update()
{
	vignette_->Update();
}

void PostEffectManager::ImguiUpdate()
{
	vignette_->ImguiUpdate();

	beatEffect_->ImguiUpdate();
}

void PostEffectManager::EffectBloomDraw()
{
	
	vignette_->Draw(dxCommon_->GetCommandList());
}

void PostEffectManager::BloomDrawSetting()
{

	postTarget_->PreDrawScene(dxCommon_->GetCommandList());

	Object3d::PreDraw(dxCommon_->GetCommandList());
	//-----ここから 3Dモデルの描画 -----//


	sceneManager_->DrawBloomObject();
	sceneManager_->Draw3D();

	//-----ここまで 3Dモデルの描画 -----//
	Object3d::PostDraw();

	postTarget_->PostDrawScene(dxCommon_->GetCommandList());



	higuLumiTarget_->PreDrawScene(dxCommon_->GetCommandList());
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//-----ここから 3Dモデルの描画 -----//


	sceneManager_->DrawHighLumiObj();
	sceneManager_->Draw3D();

	//-----ここまで 3Dモデルの描画 -----//
	Object3d::PostDraw();

	higuLumiTarget_->PostDrawScene(dxCommon_->GetCommandList());

#pragma region 高輝度抽出の描画
	postLumi_->PreDrawScene(dxCommon_->GetCommandList());

	higuLumiTarget_->Draw(dxCommon_->GetCommandList());

	postLumi_->PostDrawScene(dxCommon_->GetCommandList());
#pragma endregion

#pragma region ガウシアンブラーの描画
	gaussianBlur_->PreDrawScene(dxCommon_->GetCommandList());

	postLumi_->Draw(dxCommon_->GetCommandList());

	gaussianBlur_->PostDrawScene(dxCommon_->GetCommandList());
#pragma endregion

#pragma region ブルーム用に合成させる
	beatEffect_->DrawPass();
#pragma endregion
	
	vignette_->PreDrawScene(dxCommon_->GetCommandList());
	beatEffect_->Draw();
	
	vignette_->PostDrawScene(dxCommon_->GetCommandList());

}

void PostEffectManager::SetSceneManager(SceneManager* sceneManager)
{
	sceneManager_ = sceneManager;
}

void PostEffectManager::SetDxCommon(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	HiguLumiTarget::SetDevice(dxCommon_->GetDevice());
	PostEffectHighLumi::SetDevice(dxCommon_->GetDevice());
	GaussianBlur::SetDevice(dxCommon_->GetDevice());
	PostEffectComposition::SetDevice(dxCommon_->GetDevice());
	PostTarget::SetDevice(dxCommon_->GetDevice());
	Vignette::SetDevice(dxCommon_->GetDevice());
	PostEffectRipples::SetDevice(dxCommon_->GetDevice());
	PostEffectScaling::SetDevice(dxCommon_->GetDevice());
}
