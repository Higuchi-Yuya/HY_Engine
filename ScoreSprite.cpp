#include "ScoreSprite.h"


ScoreSprite::ScoreSprite()
{
}

ScoreSprite::~ScoreSprite()
{
    for (int i = 0; i < 10; i++)
    {
        delete scoresprite[i];
    }
}

void ScoreSprite::Initialize()
{
    NumTexHandle = Texture::LoadTexture("Numbers.png");
    for (int i = 0; i < 10; i++)
    {
        scoresprite[i] = new Sprite();
        texBase.x = texsize.x * i;
        scoresprite[i]->Initialize(NumTexHandle, pos);

        scoresprite[i]->SetTextureLeftTop({ texBase.x,texBase.y });
        scoresprite[i]->SetTextureSize({ texsize.x,texsize.y });
        scoresprite[i]->SetSize(size);
    }
}

void ScoreSprite::Draw(int num, Vector2 pos)
{
    scoresprite[num]->SetPosition({ pos.x,pos.y });
    scoresprite[num]->Draw();
}

void ScoreSprite::SetSize(Vector2 size)
{
    for (int i = 0; i < 10; i++)
    {
        scoresprite[i]->SetSize(size);
    }
}
