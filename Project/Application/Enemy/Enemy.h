#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
#include "Player.h"
#include <ParticleManager.h>

class Enemy:public Object3d
{
private:// サブクラス
	enum class State
	{
		Spawn,// 生成
		Alive,// 生きているとき
		Dead,// 死んでる時
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
	void Initialize(Model* model,Player* player);

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

	// 現在の状態
	State nowState_ = State::Spawn;

#pragma region スポーン関連
	float spawnTimer = 0;
	float spawnTimeMax = 240;
	float spawnTimeLate = 0;
	Vector3 spawnFirstPos_ = { 0,0,0 };
	Vector3 spawnEndPos_;


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
	int particleNum = 5;

#pragma endregion
};

