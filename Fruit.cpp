#include "Fruit.h"
#include "Player.h"

Fruit::Fruit()
{
}

Fruit::~Fruit()
{
	//delete model;
	delete fruitObj;
}


void Fruit::Initialize(Vector3 Pos, Model* model)
{
	// NULLポインタチェック
	assert(model);

	this->model = model;
	
	// オブジェクトの初期化
	fruitObj = Object3d::Create();

	// モデルのセット
	fruitObj->SetModel(model);

	// 初期のポジションセット
	fruitObj->worldTransform_.position_ = Pos;
	fruitObj->Update();

	start = Pos;
	p1 = { Pos.x+2,Pos.y - 3,Pos.z };
	p2 = { Pos.x-2,Pos.y - 6,Pos.z };
	end = { Pos.x,Pos.y - 16,Pos.z };

	points = { start,start,p1,p2,end,end };

	if ( rand() % 2 == 1) {
		IsSpline = true;
	}
	else {
		IsSpline = false;
	}

	isDeath = false;
}

void Fruit::Update()
{
	if (isDeath == false) {
		if (IsSpline == true) {
			nowCount++;
			// 落下
			// スタート地点		：start
			// エンド地点		：end
			// 経過時間			：elapsedTime [s]
			// 移動完了の率(経過時間/全体時間) : timeRate (%)
			elapsedTime = nowCount - startCount;
			timeRate = elapsedTime / maxTime;

			if (timeRate >= 1.0f)
			{
				if (startIndex < points.size() - 3)
				{
					startIndex += 1.0f;
					timeRate -= 1.0f;
					startCount = nowCount;
				}
				else
				{
					timeRate = 1.0f;
				}
			}

			position = splinePosition(points, startIndex, timeRate);

			fruitObj->worldTransform_.position_ = position;
		}
		else if (IsSpline == false) {
			fruitObj->worldTransform_.position_.y -= 0.2f;
		}

		// プレイヤーとの当たり判定
		if (collision.boxCollision(player_->GetPlayerPos(), fruitObj->worldTransform_.position_,
			player_->GetBasketRadius(), fruitRadius)) {
			// 当たっていたらフルーツを消す
			isDeath = true;
			isHit = true;
		}

		// 画面外処理
		if (fruitObj->worldTransform_.position_.y <= -9.0f) {
			// 画面の外にいったら消す
			isDeath = true;
		}

		// 行列の更新と転送
		fruitObj->Update();
	}
}

void Fruit::Draw(ViewProjection* view)
{
	if (isDeath == false) {
		fruitObj->Draw(view);
	}
	
}

Vector3 Fruit::splinePosition(const std::vector<Vector3>& points, size_t startIndex, float t)
{
	// 補完すべき点の数
	size_t n = points.size() - 2;

	if (startIndex > n)return points[n]; // Pnの値を返す
	if (startIndex < 1)return points[1]; // P1の値を返す

	// p0～p3の制御点を取得する　※　p1～p2を補完する
	Vector3 p0 = points[startIndex - 1];
	Vector3 p1 = points[startIndex];
	Vector3 p2 = points[startIndex + 1];
	Vector3 p3 = points[startIndex + 2];

	// Catmull - Romの式による補間
	Vector3 position = 0.5 * (2 * p1 + (-p0 + p2) * t
		+ (2 * p0 - 5 * p1 + 4 * p2 - p3) * t * t
		+ (-p0 + 3 * p1 - 3 * p2 + p3) * t * t * t);

	return position;
}
