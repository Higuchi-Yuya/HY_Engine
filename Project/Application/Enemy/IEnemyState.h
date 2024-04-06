#pragma once
#include "WorldTransform.h"
#include "Easing.h"
#include <vector>
#include "state.h"

// エネミークラスの前方宣言
class Enemy;
class Player;

class IEnemyState
{
public:
	// 純粋仮想関数
	virtual void Update(Enemy& enemy) = 0;
	virtual ~IEnemyState() {}

	// パトロール初期化
	void PatrolInit(Enemy& enemy);

	// イージングのリセット
	void EaseReset();

public:// ゲッター

	// 生存時の状態取得
	State::AliveState GetAliveState();

	// スポーン時間を取得
	float GetSpawnTimer();

	// スポーン最大時間の取得
	float GetSpawnTimeMax();

	// スポーンの最初の座標を取得
	Vector3 GetSpawnFirstPos();

	// 生存しているかどうかを取得
	bool GetIsAlive();

	// ディゾルブ処理が終わっているかどうかを取得
	bool GetIsDissolveEnd();

	// 死亡時モーションが終わっているかどうかを取得
	bool GetIsDeadMotionEnd();

	// パトロールのナンバー取得
	int32_t GetPatrolPosNum();

	// パトロールのイージングを取得
	Easing GetPatrolEase();

	// パトロールが終了してるかを取得
	bool GetIsPatrolEnd();

	// αのイージングを取得
	Easing GetAlphaEase();

	// アルファ値がぜろかどうかを取得
	bool GetIsAlphaZero();

	// 近いときのパーティクルの数を取得
	int GetNearParticleNum();

	// 近いときのタイマーを取得
	int GetNearTimer();

	// 近いときの最大タイマーを取得
	int GetNearTimeLimit();

	// 近いときの最初の色を取得
	Vector4 GetNearStartColor();

	// 近いときの最後の色を取得
	Vector4 GetNearEndColor();

public:// セッター

	// 生存時の状態を設定
	void SetAliveState(State::AliveState aliveState);

	// 生きているかどうかを設定
	void SetIsAlive(const bool IsAlive);

	// 死亡時モーションが終了しているかどうか
	void SetIsDeadMotionEnd(const bool IsDeadEnd);

	// 近いときの時間
	void SetNearTimer(int nearTimer);

	// タイマーをプラスする関数
	void IncrementNearTimer();

	// アルファフラグを設定
	void SetIsAlphaZero(bool IsAlphaZero);

	// パトロール終了フラグの設定
	void SetIsPatrolEnd(bool IsPatrolEnd);

	// プレイヤーのポインタを設定
	static void SetPlayer(Player* player);

	// パトロールのナンバーの設定
	void SetPatrolNum(int32_t patrolNum);

public:

protected:
	// 生存時の状態
	State::AliveState aState_;

	// スポーン時間
	float spawnTimer = 0;
	float spawnTimeMax = 240;
	Vector3 spawnFirstPos_ = { 0,0,0 };

	// 生きているかどうか
	bool IsAlive_ = true;

	// ディゾルブが終了しているかどうか
	bool IsDissolveEnd = false;

	// 死亡時モーションが終わっているかどうか
	bool IsDeadMotionEnd = false;

	// 借りて来るプレイヤーのポインタ
	static Player* player_;

#pragma region パトロール関連

	// パトロールする経路のポイントの座標
	std::vector<Vector3> patrolPos_;

	// パトロール座標の何番目を参照しているか
	int32_t patrolPosNum_ = 0;

	Easing easePatrol_;

	WorldTransform testW_;

	bool IsPatrolEnd_ = false;
#pragma endregion

#pragma region 戻るフェーズ関連
	Easing easeAlpha_;
	int32_t backWaitTimeLimit_ = 60;

	bool IsAlphaZero_ = false;
#pragma endregion

#pragma region プレイヤーに近いときの処理関連
	// プレイヤーに近いときに一度に出すパーティクルの数
	int nearPlayerParticleNum = 20;
	// パーティクルのタイマー
	int nearPlayerTimer = 0;
	int nearPlayerTimeLimit = 10;
	// 色の初期値
	Vector4 nearStartColor_ = { 0.35f,0.05f,0.05f,1 };
	// 色の最終値
	Vector4 nearEndColor_ = { 0.02f,0.01f,0.01f,0 };
#pragma endregion
};

