#include "ItemPaper.h"

void ItemPaper::Initialize(std::string fileName)
{
	billTex_.Initialize();
	billTex_.LoadTexture("itemEffect.png");
	billTex_.worldTransform_.translation.y = 1.5f;
	billTex_.worldTransform_.scale = { 0.2f,0.2f,0.2f };
}

void ItemPaper::Update()
{
	billTex_.Update();
}

void ItemPaper::Draw()
{
	billTex_.Draw();
}
