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
	

	// スプライトのハンドルの初期化
	AbuttonTex_.reset(TextureManager::Load2DTextureP("UI/Abutton.png"));
	BbuttonTex_.reset(TextureManager::Load2DTextureP("UI/Bbutton.png"));
	LstickTex_.reset(TextureManager::Load2DTextureP("UI/Lstic.png"));
	attackTextTex_.reset(TextureManager::Load2DTextureP("UI/findText1.png"));
	flashTextTex_.reset(TextureManager::Load2DTextureP("UI/FlashText.png"));
	moveTextTex_.reset(TextureManager::Load2DTextureP("UI/moveText1.png"));
	itemFlameTex_.reset(TextureManager::Load2DTextureP("itemKeyFlame.png"));

	Vector2 AbuttonPos(80, 480);
	Vector2 BbuttonPos(80, 540);
	LstickDefuPos_ = Vector2(80, 600);

	Vector3 AbuttonScale(0.7f, 0.7f, 0);
	Vector3 BbuttonScale(0.7f, 0.7f, 0);
	Vector3 LstickScale(1.0f, 1.0f, 0);

	// Aボタン Up の初期化
	sprites_[AButtonUp]->Initialize(AbuttonTex_.get(), AbuttonPos, Vector2(49, 48));
	sprites_[AButtonUp]->SetRectSize(Vector2(0, 0), Vector2(204, 200));
	sprites_[AButtonUp]->SetScale(AbuttonScale);

	// Aボタン Down の初期化
	sprites_[AButtonDown]->Initialize(AbuttonTex_.get(), AbuttonPos, Vector2(49, 48));
	sprites_[AButtonDown]->SetRectSize(Vector2(204, 0), Vector2(408, 200));
	sprites_[AButtonDown]->SetScale(AbuttonScale);

	// Bボタン Up の初期化
	sprites_[BButtonUp]->Initialize(BbuttonTex_.get(), BbuttonPos, Vector2(49, 48));
	sprites_[BButtonUp]->SetRectSize(Vector2(0, 0), Vector2(204, 200));
	sprites_[BButtonUp]->SetScale(BbuttonScale);

	// Bボタン Down の初期化
	sprites_[BButtonDown]->Initialize(BbuttonTex_.get(), BbuttonPos, Vector2(49, 48));
	sprites_[BButtonDown]->SetRectSize(Vector2(204, 0), Vector2(408, 200));
	sprites_[BButtonDown]->SetScale(BbuttonScale);

	// Lスティック 上 の初期化
	sprites_[LstickCover]->Initialize(LstickTex_.get(), LstickDefuPos_, Vector2(28, 48));
	sprites_[LstickCover]->SetRectSize(Vector2(0, 0), Vector2(115, 200));
	sprites_[LstickCover]->SetScale(LstickScale);

	// Lスティック 下 の初期化
	sprites_[LstickUnder]->Initialize(LstickTex_.get(), LstickDefuPos_, Vector2(46, 48));
	sprites_[LstickUnder]->SetRectSize(Vector2(115, 0), Vector2(308, 200));
	sprites_[LstickUnder]->SetScale(LstickScale);

	// 攻撃テキストの初期化
	sprites_[AttackText]->Initialize(attackTextTex_.get(), AbuttonPos + Vector2(100, -2),Vector2(360,120));
	sprites_[AttackText]->SetScale(Vector3(0.3f, 0.3f, 0));

	// フラッシュテキストの初期化
	sprites_[FlashText]->Initialize(flashTextTex_.get(), BbuttonPos + Vector2(100, -2), Vector2(480, 120));
	sprites_[FlashText]->SetScale(Vector3(0.3f, 0.3f, 0));

	// 移動テキストの初期化
	sprites_[MoveText]->Initialize(moveTextTex_.get(), LstickDefuPos_ + Vector2(100, 0), Vector2(360, 120));
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
		if (i != AButtonUp && i != AButtonDown&&
			i != BButtonUp && i != BButtonDown)
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

	if (Pad::GetButton(PadCode::ButtonB))
	{
		sprites_[BButtonDown]->Draw();
	}
	else
	{
		sprites_[BButtonUp]->Draw();
	}
}
