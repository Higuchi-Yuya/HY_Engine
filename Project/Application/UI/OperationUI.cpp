#include "OperationUI.h"
#include "JoyPadInput.h"


OperationUI::OperationUI()
{
	// スプライトのユニークポインタ作り
	for (uint32_t i = 0; i < Type::Size; i++){
		sprites_.push_back(std::move(std::make_unique<Sprite>()));
	}
}

void OperationUI::Init()
{
	LstickDefuPos_ = Vector2(100, 600);

	// スプライトのハンドルの初期化
	AbuttonTex_.reset(TextureManager::Load2DTextureP("UI/Abutton.png"));
	LstickTex_.reset(TextureManager::Load2DTextureP("UI/Lstic.png"));
	attackTextTex_.reset(TextureManager::Load2DTextureP("UI/findText.png"));
	moveTextTex_.reset(TextureManager::Load2DTextureP("UI/MoveText.png"));
	itemFlameTex_.reset(TextureManager::Load2DTextureP("itemKeyFlame.png"));

	Vector3 AbuttonScale(1.0f, 1.0f, 0);
	Vector3 LstickScale(1.5f, 1.5f, 0);

	// Aボタン Up の初期化
	sprites_[AButtonUp]->Initialize(AbuttonTex_.get(),Vector2(100, 500), Vector2(49, 48));
	sprites_[AButtonUp]->SetRectSize(Vector2(0, 0), Vector2(204, 200));
	sprites_[AButtonUp]->SetScale(AbuttonScale);

	// Aボタン Down の初期化
	sprites_[AButtonDown]->Initialize(AbuttonTex_.get(), Vector2(100, 500), Vector2(49, 48));
	sprites_[AButtonDown]->SetRectSize(Vector2(204, 0), Vector2(408, 200));
	sprites_[AButtonDown]->SetScale(AbuttonScale);

	// Lスティック 上 の初期化
	sprites_[LstickCover]->Initialize(LstickTex_.get(), LstickDefuPos_, Vector2(28, 48));
	sprites_[LstickCover]->SetRectSize(Vector2(0, 0), Vector2(115, 200));
	sprites_[LstickCover]->SetScale(LstickScale);

	// Lスティック 下 の初期化
	sprites_[LstickUnder]->Initialize(LstickTex_.get(), LstickDefuPos_, Vector2(46, 48));
	sprites_[LstickUnder]->SetRectSize(Vector2(115, 0), Vector2(308, 200));
	sprites_[LstickUnder]->SetScale(LstickScale);

	// 攻撃テキストの初期化
	sprites_[AttackText]->Initialize(attackTextTex_.get(), Vector2(100, 500) + Vector2(100, -2),Vector2(360,120));
	sprites_[AttackText]->SetScale(Vector3(0.3f, 0.3f, 0));

	// 移動テキストの初期化
	sprites_[MoveText]->Initialize(moveTextTex_.get(), LstickDefuPos_ + Vector2(100, 0), Vector2(272, 120));
	sprites_[MoveText]->SetScale(Vector3(0.3f, 0.3f, 0));

	// アイテムフレームの初期化
	itemFlameDefaPos_ = { 100,150 };
	itemFlameDefaSize_ = { 70,70 };
	float abjustmentPos = 10;

	sprites_[itemFlame1]->Initialize(itemFlameTex_.get(), { itemFlameDefaPos_ },itemFlameDefaSize_);
	sprites_[itemFlame2]->Initialize(itemFlameTex_.get(), { (itemFlameDefaPos_.x + itemFlameDefaSize_.x * 1)+ abjustmentPos,
															itemFlameDefaPos_.y }, itemFlameDefaSize_);
	sprites_[itemFlame3]->Initialize(itemFlameTex_.get(), { (itemFlameDefaPos_.x + itemFlameDefaSize_.x * 2) + abjustmentPos * 2,
															itemFlameDefaPos_.y }, itemFlameDefaSize_);
}

void OperationUI::Update()
{
	// 左スティックの処理
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
