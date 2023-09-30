#include "OperationUI.h"
#include "JoyPadInput.h"

OperationUI::OperationUI()
{
	// �X�v���C�g�̃��j�[�N�|�C���^���
	for (uint32_t i = 0; i < Type::Size; i++){
		sprites_.push_back(std::move(std::make_unique<Sprite>()));
	}
}

void OperationUI::Init()
{
	LstickDefuPos_ = Vector2(100, 600);

	// �X�v���C�g�̃n���h���̏�����
	AbuttonTex_.reset(TextureManager::Load2DTextureP("UI/Abutton.png"));
	LstickTex_.reset(TextureManager::Load2DTextureP("UI/Lstic.png"));
	attackTextTex_.reset(TextureManager::Load2DTextureP("UI/AttackText.png"));
	moveTextTex_.reset(TextureManager::Load2DTextureP("UI/MoveText.png"));

	Vector3 AbuttonScale(1.0f, 1.0f, 0);
	Vector3 LstickScale(1.5f, 1.5f, 0);

	// A�{�^�� Up �̏�����
	sprites_[AButtonUp]->Initialize(AbuttonTex_.get(),Vector2(100, 500), Vector2(49, 48));
	sprites_[AButtonUp]->SetRectSize(Vector2(0, 0), Vector2(204, 200));
	sprites_[AButtonUp]->SetScale(AbuttonScale);

	// A�{�^�� Down �̏�����
	sprites_[AButtonDown]->Initialize(AbuttonTex_.get(), Vector2(100, 500), Vector2(49, 48));
	sprites_[AButtonDown]->SetRectSize(Vector2(204, 0), Vector2(408, 200));
	sprites_[AButtonDown]->SetScale(AbuttonScale);

	// L�X�e�B�b�N �� �̏�����
	sprites_[LstickCover]->Initialize(LstickTex_.get(), LstickDefuPos_, Vector2(28, 48));
	sprites_[LstickCover]->SetRectSize(Vector2(0, 0), Vector2(115, 200));
	sprites_[LstickCover]->SetScale(LstickScale);

	// L�X�e�B�b�N �� �̏�����
	sprites_[LstickUnder]->Initialize(LstickTex_.get(), LstickDefuPos_, Vector2(46, 48));
	sprites_[LstickUnder]->SetRectSize(Vector2(115, 0), Vector2(308, 200));
	sprites_[LstickUnder]->SetScale(LstickScale);

	// �U���e�L�X�g�̏�����
	sprites_[AttackText]->Initialize(attackTextTex_.get(), Vector2(100, 500) + Vector2(100, -2),Vector2(360,120));
	sprites_[AttackText]->SetScale(Vector3(0.3f, 0.3f, 0));

	// �ړ��e�L�X�g�̏�����
	sprites_[MoveText]->Initialize(moveTextTex_.get(), LstickDefuPos_ + Vector2(100, 0), Vector2(272, 120));
	sprites_[MoveText]->SetScale(Vector3(0.3f, 0.3f, 0));
}

void OperationUI::Update()
{
	Vector2 leftStick = Pad::GetStick(PadCode::LeftStick, 300);
	if (leftStick != 0)
	{
		Vector2 mMoveVec =
		{
			leftStick.x,
			leftStick.y
		};

		sprites_[LstickCover]->SetPosition(LstickDefuPos_ + mMoveVec.normalize() * 20);
	}
	else
	{
		sprites_[LstickCover]->SetPosition(LstickDefuPos_);
	}
}

void OperationUI::DrawFrontSprite()
{
	for (uint32_t i = 0; i < Type::Size; i++)
	{
		if (i != AButtonUp && i != AButtonDown)
		{
			sprites_[i]->Draw();
		}
	}
	if (Pad::GetButton(PadCode::ButtonA))
	{
		sprites_[AButtonDown]->Draw();
	}
	else
	{
		sprites_[AButtonUp]->Draw();
	}
}
