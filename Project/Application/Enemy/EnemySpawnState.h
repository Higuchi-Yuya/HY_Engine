#pragma once
#include"IEnemyState.h"

class EnemySpawnState:public IEnemyState
{

public:

	// 更新処理
	void Update(Enemy& enemy)override;


private:

#pragma region スポーン関連

	float spawnTimeLate = 0;
	Vector3 spawnEndPos_;
	Easing ease_;

#pragma endregion
};

