#include "GameOverScene.h"
#include "InputManager.h"

void GameOverScene::Initialize()
{
	// タイトルの背景で使うハンドル（仮）
	backTexHandle_.reset(TextureManager::Load2DTextureP("titleBack.png"));
	// タイトルのAボタン押すフォント（仮）
	buttonTexHandle_.reset(TextureManager::Load2DTextureP("PressA.png"));
	// リザルト画面のゲームクリアのフォント
	resultGameOverFontHandle_.reset(TextureManager::Load2DTextureP("Result/GameOverFont.png"));

	// 背景のスプライト
	backSprite_ = std::make_unique<Sprite>();
	backSprite_->Initialize(backTexHandle_.get(), { WinApp::window_width / 2,WinApp::window_height / 2 }, { 1280,720 });
	// タイトルのボタンAのスプライト
	pressASprite = std::make_unique<Sprite>();
	pressASprite->Initialize(buttonTexHandle_.get(), { WinApp::window_width / 2,WinApp::window_height / 2 + 200 }, { 250,50 });
	// リザルトのゲームオーバーフォントスプライト
	resultGameOverFont_ = std::make_unique<Sprite>();
	resultGameOverFont_->Initialize(resultGameOverFontHandle_.get(), { WinApp::window_width / 2,WinApp::window_height / 2 - 100 }, { 500,250 });
}

void GameOverScene::Update()
{
	// パッドでAボタンを押した瞬間
	if (JoypadInput::GetButtonDown(PadCode::ButtonA)) {
		IsSceneFinsh_ = true;
	}
}

void GameOverScene::Draw3D()
{
}

void GameOverScene::Draw2DFront()
{
	backSprite_->Draw();
	pressASprite->Draw();
	resultGameOverFont_->Draw();
}

void GameOverScene::Reset()
{
	IsSceneFinsh_ = false;
	IsTitleOrGame_ = true;
}

void GameOverScene::ImguiUpdate()
{
}

void GameOverScene::DrawBloomObject()
{
}

void GameOverScene::DrawHighLumiObj()
{
}

bool GameOverScene::GetIsGameClear()
{
	return false;
}
