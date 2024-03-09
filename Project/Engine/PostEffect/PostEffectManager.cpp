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

	beatEffect_ = std::make_unique<BeatEffect>();
	beatEffect_->Initialize();
	BeatEffect::SetGaussianBlur(gaussianBlur_.get());

	boxModel_.reset(Model::LoadFromOBJ("CharacterYomawari"));
	testDObj.reset(DeferredObject3d::Create());
	testDObj->SetModel(boxModel_.get());

	testDefrredRender_ = std::make_unique<DeferredRender>();
	testDefrredRender_->Initialize();

}

void PostEffectManager::Update()
{
	//beatEffect_->Update();
	testDObj->Update();
	testDefrredRender_->Update();
}

void PostEffectManager::ImguiUpdate()
{
	

	beatEffect_->ImguiUpdate();
}

void PostEffectManager::EffectBloomDraw()
{
	beatEffect_->Draw();

}

void PostEffectManager::BloomDrawSetting()
{
	// ------- ディファード用のオブジェクト ------- //
	DeferredObject3d::PreDraw(dxCommon_->GetCommandList());
	testDObj->Draw(&sceneManager_->GetGameCamera());
	sceneManager_->DrawDefrred3D();
	sceneManager_->DrawBloomObject();

	DeferredObject3d::PostDraw();

	// ------ ディファードレンダリング --------- //
	testDefrredRender_->PreDrawScene(dxCommon_->GetCommandList());

	DeferredObject3d::DrawDeferredObjRenderTex();

	testDefrredRender_->PostDrawScene();


	// ------ ターゲットシーンの描画 ------ //
	postTarget_->PreDrawScene(dxCommon_->GetCommandList());

	testDefrredRender_->Draw();

	Object3d::PreDraw(dxCommon_->GetCommandList());
	sceneManager_->DrawForward3D();
	Object3d::PostDraw();

	postTarget_->PostDrawScene(dxCommon_->GetCommandList());

	//higuLumiTarget_->PreDrawScene(dxCommon_->GetCommandList());
	//Object3d::PreDraw(dxCommon_->GetCommandList());
	////-----ここから 3Dモデルの描画 -----//


	//sceneManager_->DrawHighLumiObj();
	//sceneManager_->Draw3D();

	////-----ここまで 3Dモデルの描画 -----//
	//Object3d::PostDraw();

	//higuLumiTarget_->PostDrawScene(dxCommon_->GetCommandList());


	
#pragma region 高輝度抽出の描画
	postLumi_->PreDrawScene(dxCommon_->GetCommandList());
	
	DeferredObject3d::PreDraw(dxCommon_->GetCommandList());
	sceneManager_->DrawBloomObject();

	DeferredObject3d::PostDraw();
	//higuLumiTarget_->Draw(dxCommon_->GetCommandList());

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
	DeferredRender::SetDevice(dxCommon_->GetDevice());

	DeferredObject3d::SetDsvHandle(dxCommon_->GetDepthCpuHandle());
	DeferredRender::SetDsvHandle(dxCommon_->GetDepthCpuHandle());
	PostTarget::SetDsvHandle(dxCommon_->GetDepthCpuHandle());
	PostEffectHighLumi::SetDsvHandle(dxCommon_->GetDepthCpuHandle());

}
