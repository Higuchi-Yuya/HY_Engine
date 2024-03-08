#include "GameClearScene.h"
#include "InputManager.h"

void GameClearScene::Initialize()
{
	// タイトルの背景で使うハンドル（仮）
	backTexHandle_.reset(TextureManager::Load2DTextureP("titleBack.png"));
	// タイトルのAボタン押すフォント（仮）
	buttonTexHandle_.reset(TextureManager::Load2DTextureP("PressA.png"));
	// リザルト画面のゲームクリアのフォント
	resultGameClearFontHandle_.reset(TextureManager::Load2DTextureP("Result/GameClearFont.png"));

	// 背景のスプライト
	backSprite_ = std::make_unique<Sprite>();
	backSprite_->Initialize(backTexHandle_.get(), { WinApp::window_width / 2,WinApp::window_height / 2 }, { 1280,720 });
	// タイトルのボタンAのスプライト
	pressASprite = std::make_unique<Sprite>();
	pressASprite->Initialize(buttonTexHandle_.get(), { WinApp::window_width / 2,WinApp::window_height / 2 + 200 }, { 250,50 });
	// リザルトのゲームクリアフォントスプライト
	resultGameClearFont_ = std::make_unique<Sprite>();
	resultGameClearFont_->Initialize(resultGameClearFontHandle_.get(), { WinApp::window_width / 2,WinApp::window_height / 2 - 100 }, { 500,250 });
}

void GameClearScene::Update()
{
	// パッドでAボタンを押した瞬間
	if (JoypadInput::GetButtonDown(PadCode::ButtonA)) {
		IsSceneFinsh_ = true;
	}
}

void GameClearScene::DrawForward3D()
{
}

void GameClearScene::DrawDefrred3D()
{
}

void GameClearScene::Draw2DFront()
{
	backSprite_->Draw();
	pressASprite->Draw();
	resultGameClearFont_->Draw();
}

void GameClearScene::ImguiUpdate()
{
}

void GameClearScene::DrawBloomObject()
{
}

void GameClearScene::DrawHighLumiObj()
{
}

bool GameClearScene::GetIsGameClear()
{
	return false;
}

void GameClearScene::Reset()
{
	IsSceneFinsh_ = false;
	IsTitleOrGame_ = true;
}
