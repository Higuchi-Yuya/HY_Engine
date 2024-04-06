#include "EnemyAliveState.h"
#include "Enemy.h"


//EnemyAliveState::~EnemyAliveState()
//{
//	delete cureentAliveState_;
//}

void EnemyAliveState::Update(Enemy& enemy)
{
	AliveUpdate(enemy);
}

void EnemyAliveState::Init(Enemy& enemy)
{
	aPatrol_.reset(new EnemyAlivePatrol());
	aPatrol_->Init(enemy);
	aBack_.reset(new EnemyAliveBack());
	aBack_->Init(enemy);
	aTracking_.reset(new EnemyAliveTracking());
	aTracking_->Init(enemy);
	aDead_.reset(new EnemyAliveDead());
	aDead_->Init(enemy);

	cureentAliveState_ = aPatrol_.get();
	cureentAliveState_->Init(enemy);
	
}

void EnemyAliveState::AliveUpdate(Enemy& enemy)
{
	if (IsInit_ == false) {
		Init(enemy);
		IsInit_ = true;
	}

	// 死んだら状態を変更
	if (IsAlive_ == false) {
		aState_ = State::AliveState::Dead;
	}

	IsDissolveEnd = cureentAliveState_->GetIsDissolveEnd();

	// 生きている状態が変更されているかチェック
	ChangeStateCheck();

	cureentAliveState_->SetValue(&easePatrol_, &easeAlpha_);

	// 生きている状態の更新処理
	cureentAliveState_->UpdateAlive(enemy,&aState_);
}

bool EnemyAliveState::ChangeStateCheck()
{
	switch (aState_)
	{
	case State::AliveState::Patrol:// 巡回
		cureentAliveState_ = aPatrol_.get();

		break;
	case State::AliveState::Back:// 戻るとき
		cureentAliveState_ = aBack_.get();

		break;
	case State::AliveState::Tracking:// 追跡
		cureentAliveState_ = aTracking_.get();

		break;
	case State::AliveState::Dead:
		cureentAliveState_ = aDead_.get();

		break;
	}

	return true;
}
