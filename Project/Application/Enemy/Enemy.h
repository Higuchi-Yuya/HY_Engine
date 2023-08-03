#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
#include "Player.h"

class Enemy:public Object3d
{
public:
	// 初期化
	void Initialize(Model* model,Player* player);

	// 毎フレーム処理
	void Update()override;

	// 描画
	void Draw(ViewProjection* view)override;

	// 衝突時コールバック関数
	void OnCollision();

	const bool GetAlive() { return IsAlive_; }

	// ワールドトランスフォームの情報をセット
	void SetWorldTransInfo(WorldTransform worldTrans);

	/// <summary>
	/// 生きているかフラグのセット
	/// </summary>
	/// <param name="isAlive">生きているかフラグ</param>
	void SetAlive(bool isAlive) { IsAlive_ = isAlive; }


private:
	// 借りてくるプレイヤーの情報を一時的に保存
	Player* player_ = nullptr;

	// 当たったかどうか
	bool IsCollision = false;

	// 生きているかどうか
	bool IsAlive_ = false;

	// ディゾルブ関連
	float disoTimer_ = 0;
	float disoTimeMax_ = 60 * 3;
	float disoTimeLate_ = 0;

	// フラグ
	bool isH = false;

	// 追従関連
	float followTimer = 0;
	float followTimeMax = 20;
	Vector3 followVec;
	float followSpeed = 0.1f;

	float timer = 0;
	float maxTime = 240;
};

