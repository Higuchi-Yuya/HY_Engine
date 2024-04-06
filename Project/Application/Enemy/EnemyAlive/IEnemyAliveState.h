#pragma once
#include "IEnemyState.h"
#include "state.h"

class Player;

class IEnemyAliveState
{
public:
	// 更新処理
	virtual void UpdateAlive(Enemy& enemy, State::AliveState* aliveState) = 0;
	virtual ~IEnemyAliveState() {};

	// 初期化
	void Init(Enemy& enemy);

	bool GetIsDissolveEnd();

	// 値の設定
	void SetValue(Easing* easeP,Easing* easeA);

	// プレイヤーのポインタを設定
	static void SetPlayer(Player* player);

protected:
	// 借りて来るプレイヤーのポインタ
	static Player* player_;

	// ディゾルブが終了しているかどうか
	bool IsDissolveEnd = false;

#pragma region パトロール関連

	// パトロールする経路のポイントの座標
	std::vector<Vector3> patrolPos_;

	// パトロール座標の何番目を参照しているか
	int32_t patrolPosNum_ = 0;

	Easing* easePatrol_;

	WorldTransform testW_;

	bool IsPatrolEnd_ = false;
#pragma endregion

#pragma region 戻るフェーズ関連
	Easing* easeAlpha_;

	int32_t backWaitTimeLimit_ = 60;

	bool IsAlphaZero_ = false;
#pragma endregion
};

