#include "PostEffectManager.h"

void PostEffectManager::Initialize()
{
	postLumi_ = std::make_unique<PostEffectHighLumi>();
	postLumi_->Initialize();
}

void PostEffectManager::EffectBloomDraw()
{
	postLumi_->Draw(dxCommon_->GetCommandList());
}

void PostEffectManager::BloomDrawSetting()
{
	postLumi_->PreDrawScene(dxCommon_->GetCommandList());
	Object3d::PreDraw(dxCommon_->GetCommandList());
	//-----ここから 3Dモデルの描画 -----//


	gameScene_->Draw3D();


	//-----ここまで 3Dモデルの描画 -----//
	Object3d::PostDraw();
	

	postLumi_->PostDrawScene(dxCommon_->GetCommandList());
}

void PostEffectManager::SetGameScene(GameScene* gameScene)
{
	gameScene_ = gameScene;
}

void PostEffectManager::SetDxCommon(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	PostEffectHighLumi::SetDevice(dxCommon_->GetDevice());

}
