#pragma once
#include "Sprite.h"
class ScoreSprite
{
public:
    ScoreSprite();
    ~ScoreSprite();

    // 初期化の関数
    void Initialize();

    // 描画の関数
    void Draw(int num, Vector2 pos);

    void SetSize(Vector2 size);

    const Vector2& GetSize() const { return size; }

private:
    // スプライト
    int NumTexHandle = 0;
    Sprite* scoresprite[10]{};

    // 初期化用変数
    Vector4 color = { 1, 1, 1, 1 };
    Vector2 anker = { 0, 0 };
    Vector2 size = { 40,50 };
    //スプライト一枚あたりのサイズ
    const Vector2 texsize = { 64,80 };
    Vector2 texBase = { 0,0 };

    // 位置
    Vector2 pos = { 800,200 };
};

