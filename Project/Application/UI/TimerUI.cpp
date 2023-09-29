#include "TimerUI.h"
#include "Util.h"
#include "WinApp.h"

TimerUI::TimerUI()
{
	// �^�C�}�[�p�̃e�N�X�`���̃��[�h
	numberTex_.reset(TextureManager::Load2DTextureP("NumberSheet.png"));
}

void TimerUI::Init()
{
	mParent.Initialize();
	mParent.translation = Vector3(WinApp::window_width -150, 80, 0);
	mParent.scale = Vector3(0.8f,0.8f,1.0f);
	mParent.UpdateMatrix();

	// �^�C�}�[�̃X�v���C�g�̏�����
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
	// �^�C�}�[�̐����ɍ��킹�ăe�N�X�`���̐ݒ��ύX
	TextureSetting();

	// �^�C�}�[�̐����̃|�W�V���������ɂ��炷
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
	// �^�C�}�[�̐����̕`��
	for (uint32_t i = 0; i < dight; i++)
	{
		mTimerUnit[i]->Draw();
	}
}

void TimerUI::Reset()
{
	// �^�C�}�[�̐����̃��Z�b�g
	gameTimer_ = 0;
	remainTime_ = 0;
}

void TimerUI::TextureSetting()
{
	gameTimer_++;

	if (gameTimer_ >= 60)
	{
		gameTimer_ = 0;
		remainTime_--;
	}
	
	dight = Util::GetDight(remainTime_);

	// 0�̎�
	if (remainTime_ <= 0)
	{
		for (uint32_t i = 0; i < mTimerUnit.size(); i++)
		{
			mTimerUnit[i]->SetRectSize(Vector2(0, 0), Vector2(128, 128));
		}
		return;
	}

	for (uint32_t i = 0; i < dight; i++)
	{
		// ���݂̌��̐����擾
		uint32_t num = Util::GetDightNumber(remainTime_, dight - i);

		// �S�̌�����e�N�X�`���[�Z�b�g����
		mTimerUnit[i]->SetRectSize(
			Vector2(0 + num * mTexSize, 0), Vector2(128 + num * mTexSize, mTexSize));
	}
}