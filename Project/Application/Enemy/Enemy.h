#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
#include "Player.h"
#include <ParticleManager.h>


class Enemy:public Object3d
{
public:// サブクラス
	enum class State
	{
		Spawn,// 生成
		Alive,// 生きているとき
		Dead,// 死んでる時
	};

	enum AliveState
	{
		Patrol,//巡回
		Back,// 戻る
		Tracking,//追尾
	};

public:
	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// 静的解放
	/// </summary>
	static void StaticFinalize();

	// 初期化
	void Initialize(Model* model = nullptr);

	// 毎フレーム処理
	void Update()override;

	// パーティクルの更新処理
	void ParticleUpdate();

	// 描画
	void Draw(ViewProjection* view)override;

	// パーティクル描画
	void DrawParticle(ViewProjection* view);

	// 衝突時コールバック関数
	void OnCollision();

	// 押し戻し衝突判定コールバック関数
	void pushBackOnCol();

	// 戻るフェーズ関連をリセット
	void ResetBack();

public:// ゲッター
	// 生きているかを取得
	const bool GetAlive() { return IsAlive_; }

	// 死亡モーションが全て終わっているかを取得
	const bool GetDeadMotionEnd() { return IsDeadMotionEnd; }

	// 今の状態を取得
	const State GetState() { return nowState_; }

	// 生きているときの今の状態を取得
	const AliveState GetAliveState() { return aliveState_; }

	// 移動前のポジションの取得
	const Vector3 GetOldPos() { return oldPos_; }

public:// セッター
	// ワールドトランスフォームの情報をセット
	void SetWorldTransInfo(WorldTransform worldTrans);

	// ポジションのセット
	void SetWorldPos(Vector3 pos);

	/// <summary>
	/// 生きているかフラグのセット
	/// </summary>
	/// <param name="isAlive">生きているかフラグ</param>
	void SetAlive(bool isAlive) { IsAlive_ = isAlive; }

	/// <summary>
	/// プレイヤーを設定
	/// </summary>
	/// <param name="player">プレイヤーのポインタ</param>
	static void SetPlayer(Player* player) { player_ = player; }

	/// <summary>
	/// 敵の生きているときの行動状態を設定
	/// </summary>
	/// <param name="aState">状態</param>
	void SetAliveState(AliveState aState) { aliveState_ = aState; }

private:// プライベート関数

	/// <summary>
	/// 生きている時の更新処理
	/// </summary>
	void AliveUpdate();

	/// <summary>
	/// 巡回の更新処理
	/// </summary>
	void PatrolUpdate();

	/// <summary>
	/// 移動の更新処理
	/// </summary>
	void TrackingUpdate();

	/// <summary>
	/// 死亡時の更新処理
	/// </summary>
	void DeadUpdate();

public:
	// 衝突点
	Vector3 interPos;
	// 排斥ベクトル
	Vector3 rejectVec;

private:// 静的メンバ変数
	// 借りてくるプレイヤーの情報を一時的に保存
	static Player* player_;

	// デフォルトのモデル
	static std::unique_ptr<Model> modelDefu_;

private:


	// 当たったかどうか
	bool IsCollision = false;

	// 生きているかどうか
	bool IsAlive_ = false;

	// 死亡時モーションが終わっているかどうか
	bool IsDeadMotionEnd = false;

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

	// 現在の状態
	State nowState_ = State::Spawn;

	// ライフ
	uint32_t nowLife_ = 0;
	const uint32_t maxLife_ = 3;

	// 移動する前の古いポジション
	Vector3 oldPos_{};

	// 追いかけて来るかどうか
	bool IsChase = false;
	
	// 生きてる時の状態推移
	AliveState aliveState_ = AliveState::Patrol;

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


#pragma region スポーン関連
	float spawnTimer = 0;
	float spawnTimeMax = 240;
	float spawnTimeLate = 0;
	Vector3 spawnFirstPos_ = { 0,0,0 };
	Vector3 spawnEndPos_;
	Easing ease_;

#pragma endregion


#pragma region パーティクル
	// パーティクルのテクスチャ
	static std::unique_ptr<Texture> sParticleTex_;
	// パーティクルマネージャ
	std::unique_ptr<ParticleManager> particleMan_;

	// パーティクルの色
	// 色の初期値
	Vector4 startColor_ = { 1,0.05f,0.05f,1 };
	// 色の最終値
	Vector4 endColor_ = { 0,0,0,0 };
	// 一度にだすパーティクルの数
	int particleNum = 15;

#pragma endregion
};

