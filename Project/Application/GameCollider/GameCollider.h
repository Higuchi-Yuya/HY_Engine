#pragma once
#include "Enemy.h"
#include "Player.h"
#include "ParticleManager.h"

class GameCollider
{
public:// メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Updata();

	/// <summary>
	/// エネミーと当たったらプレイヤーの中心にパーティクルを出す処理
	/// </summary>
	void OnColParticle();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(ID3D12GraphicsCommandList* commandList, ViewProjection* viewProjection);

	/// <summary>
	/// エネミーの情報をベクターに格納
	/// </summary>
	/// <param name="enemy">エネミー</param>
	void AddEnemy(Enemy* enemy);

	/// <summary>
	/// プレイヤーを取得する
	/// </summary>
	/// <param name="player">プレイヤー</param>
	void SetPlayer(Player* player);

private:// プライベート関数


private:// メンバ変数

	int playerHitTimer = 0;
	int playerHitTimeMax = 60;
	bool isPlayerHit = false;
	bool isPartile = false;

	std::vector<Enemy*>enemysInfo_;
	Player* player_ = nullptr;

#pragma region パーティクル
	// パーティクルのテクスチャ
	std::unique_ptr<Texture> particleTex_;
	// パーティクルマネージャ
	std::unique_ptr<ParticleManager> particleMan_;

	// パーティクルの色
	// 色の初期値
	Vector4 startColor_ = { 1,0,0,1 };
	// 色の最終値
	Vector4 endColor_ = { 1,1,1,0 };
	// 一度にだすパーティクルの数
	int particleNum = 50;

#pragma endregion

};

