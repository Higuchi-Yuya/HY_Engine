#pragma once
#include <cassert>
#include "Object3d.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Collision.h"
class Player;

// フルーツのタイプ
enum class fruitType {
	apple,
	orange,
	banana,
	bomb
};
class Fruit
{

public:
	Fruit();
	~Fruit();

	// 初期化
	void Initialize(Vector3 Pos,Model* model);
	// 更新処理
	void Update();
	// 描画処理
	void Draw(ViewProjection* view);

	void SetPlayer(Player* player) { player_ = player; }

	// 死亡フラグを渡す
	bool IsDeath() const { return isDeath; }

	// ヒットフラグを渡す
	bool IsHit()const { return isHit; }

	// 加算フラグを渡す
	bool IsPlus() const { return isPlus; }

	void SetIsPlus(bool isPlus) { this->isPlus = isPlus; }

	// 種類の番号
	int typeNum = 0;

	Vector3 splinePosition(const std::vector<Vector3>& points, size_t startIndex, float t);

private:
	// モデル
	Model* model = nullptr;

	// オブジェクト
	Object3d* fruitObj = nullptr;

	// 消滅フラグ
	bool isDeath = false;

	// 衝突フラグ
	bool isHit = false;

	// 点数が加算されかのフラグ
	bool isPlus = false;

	// フルーツの一個の半径
	Vector3 fruitRadius = { 1.0f,0.8f,1.0f };

	// 当たり判定
	Collision collision;

	Player* player_ = nullptr;

	bool IsSpline = false;
#pragma region スプライン曲線
	// スタートのポジション
	Vector3 start;
	// 途中のポジション１
	Vector3 p1;
	// 途中のポジション２
	Vector3 p2;
	// 終了のポジション
	Vector3 end;

	std::vector<Vector3> points;
	size_t startIndex = 1;
	// 移動の位置
	Vector3 position;

	// スタートする時間
	float startCount = 0;
	// 今の時間
	float nowCount = 0;
	// 経過時間
	float elapsedTime = 0;
	// 全体の時間
	float maxTime = 1.2f * 60;
	// 移動完了の率(経過時間 / 全体時間) : timeRate(%)
	float timeRate;
#pragma endregion

};

