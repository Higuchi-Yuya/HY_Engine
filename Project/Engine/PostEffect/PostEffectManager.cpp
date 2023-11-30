#include "PostEffectManager.h"

void PostEffectManager::Initialize()
{
	higuLumiTarget_ = std::make_unique<HiguLumiTarget>();
	higuLumiTarget_->Initialize();

	postLumi_ = std::make_unique<PostEffectHighLumi>();
	postLumi_->Initialize();

	gaussianBlur_ = std::make_unique<GaussianBlur>();
	gaussianBlur_->Initialize();

	postComposition_ = std::make_unique<PostEffectComposition>();
	postComposition_->Initialize();

	postTarget_= std::make_unique<PostTarget>();
	postTarget_->Initialize();

	vignette_ = std::make_unique<Vignette>();
	vignette_->Initialize();
}

void PostEffectManager::Update()
{
	vignette_->Update();
}

void PostEffectManager::ImguiUpdate()
{
	vignette_->ImguiUpdate();
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


	gameScene_->DrawBloomObject();
	gameScene_->Draw3D();

	//-----ここまで 3Dモデルの描画 -----//
	Object3d::PostDraw();

	postTarget_->PostDrawScene(dxCommon_->GetCommandList());



	higuLumiTarget_->PreDrawScene(dxCommon_->GetCommandList());
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//-----ここから 3Dモデルの描画 -----//


	gameScene_->DrawHighLumiObj();
	gameScene_->Draw3D();

	//-----ここまで 3Dモデルの描画 -----//
	Object3d::PostDraw();

	higuLumiTarget_->PostDrawScene(dxCommon_->GetCommandList());

#pragma region 高輝度抽出の描画
	postLumi_->PreDrawScene(dxCommon_->GetCommandList());

	higuLumiTarget_->Draw(dxCommon_->GetCommandList());
	//Object3d::PreDraw(dxCommon_->GetCommandList());
	//gameScene_->DrawBloomObject();

	//Object3d::PostDraw();

	postLumi_->PostDrawScene(dxCommon_->GetCommandList());
#pragma endregion

#pragma region ガウシアンブラーの描画
	gaussianBlur_->PreDrawScene(dxCommon_->GetCommandList());

	postLumi_->Draw(dxCommon_->GetCommandList());

	gaussianBlur_->PostDrawScene(dxCommon_->GetCommandList());
#pragma endregion

#pragma region ブルーム用に合成させる
	postComposition_->PreDrawScene(dxCommon_->GetCommandList());

	gaussianBlur_->Draw(dxCommon_->GetCommandList());

	postComposition_->PostDrawScene(dxCommon_->GetCommandList());
#pragma endregion
	
	vignette_->PreDrawScene(dxCommon_->GetCommandList());
	postComposition_->Draw(dxCommon_->GetCommandList());
	

	vignette_->PostDrawScene(dxCommon_->GetCommandList());

}

void PostEffectManager::SetGameScene(GameScene* gameScene)
{
	gameScene_ = gameScene;
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
}
