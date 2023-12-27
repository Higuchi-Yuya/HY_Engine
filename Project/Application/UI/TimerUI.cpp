#include "TimerUI.h"
#include "Util.h"
#include "WinApp.h"
#include "ImGuiManager.h"

TimerUI::TimerUI()
{
	// タイマー用のテクスチャのロード
	numberTex_.reset(TextureManager::Load2DTextureP("NumberSheet.png"));
}

void TimerUI::Init()
{
	mParent.Initialize();
	mParent.translation = Vector3(WinApp::window_width -150, 80, 0);
	mParent.scale = Vector3(0.8f,0.8f,1.0f);
	mParent.UpdateMatrix();

	// タイマーのスプライトの初期化
	for (uint32_t i = 0; i < mTimerUnit.size(); i++)
	{
		mTimerUnit[i] = std::make_unique<Sprite>();
		mTimerUnit[i]->Initialize(numberTex_.get());

		mTimerUnit[i]->SetRectSize(Vector2(0, 0), Vector2(128, 128));
		mTimerUnit[i]->SetSize(Vector2(mTexSize, mTexSize));
		mTimerUnit[i]->SetPosition(Vector2(((float)i - 1) * mTexSpace, 0));
		mTimerUnit[i]->SetScale(Vector3(0.4f, 0.4f,0.0f));
	}
}

void TimerUI::Update()
{
	// タイマーの数字に合わせてテクスチャの設定を変更
	TextureSetting();

	// タイマーの数字のポジションを横にずらす
	for (uint32_t i = 0; i < mTimerUnit.size(); i++)
	{
		float offset = (dight - 1) * ((mTexSpace * mTimerUnit[i]->GetScale().x) / 2.f);
		mTimerUnit[i]->SetPosition(Vector2(mParent.translation.x + ((float)i * mTexSpace * mTimerUnit[i]->GetScale().x - offset),
			mParent.translation.y));
		mTimerUnit[i]->Updata();
	}
}

void TimerUI::DrawFrontSprite()
{
	// タイマーの数字の描画
	for (uint32_t i = 0; i < dight; i++)
	{
		mTimerUnit[i]->Draw();
	}
}

void TimerUI::DrawImgui()
{
	// 表示項目の追加--------//
	ImGui::Begin("Time");

	//ImGui::SetWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(500, 100));

	ImGui::InputInt("remainTime", &remainTime_, 0, 120);


	ImGui::End();
}

void TimerUI::Reset()
{
	// タイマーの数字のリセット
	gameTimer_ = 0;
	remainTime_ = 60 * 2;

	IsTimeEnd_ = false;
}

void TimerUI::TextureSetting()
{
	gameTimer_++;
	int32_t gameTimeLimit = 60;
	if (gameTimer_ >= gameTimeLimit)
	{
		gameTimer_ = 0;
		remainTime_--;
	}
	
	dight = Util::GetDight(remainTime_);

	// 0の時
	if (remainTime_ <= 0)
	{
		for (uint32_t i = 0; i < mTimerUnit.size(); i++)
		{
			mTimerUnit[i]->SetRectSize(Vector2(0, 0), Vector2(128, 128));
		}

		// 時間が終了したときのフラグをオン
		IsTimeEnd_ = true;
		return;
	}

	for (uint32_t i = 0; i < dight; i++)
	{
		// 現在の桁の数を取得
		uint32_t num = Util::GetDightNumber(remainTime_, dight - i);

		// 百の桁からテクスチャーセットする
		mTimerUnit[i]->SetRectSize(
			Vector2(0 + num * mTexSize, 0), Vector2(128 + num * mTexSize, mTexSize));
	}
}