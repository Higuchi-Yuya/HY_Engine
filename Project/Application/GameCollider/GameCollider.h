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
	void Draw(ViewProjection* viewProjection);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw3D(ViewProjection* viewProjection);

	/// <summary>
	/// エネミーの情報をベクターに格納
	/// </summary>
	/// <param name="enemy">エネミー</param>
	void AddEnemy(Enemy* enemy);

	/// <summary>
	/// オブジェクトの情報をベクター型に格納
	/// </summary>
	/// <param name="obj">オブジェクト</param>
	void AddObj(Object3d* obj);

	/// <summary>
	/// プレイヤーを取得する
	/// </summary>
	/// <param name="player">プレイヤー</param>
	void SetPlayer(Player* player);

	/// <summary>
	/// リセット関数
	/// </summary>
	void Reset();

private:// プライベート関数


private:// メンバ変数

	int playerHitTimer = 0;
	int playerHitTimeMax = 60;
	bool isPlayerHit = false;
	bool isPartile = false;

	// エネミーの当たり判定用配列
	std::vector<Enemy*>enemysInfo_;
	// プレイヤーバレット当たり判定用の配列
	std::list<std::unique_ptr<PlayerBullet>> bulletsInfo_;
	// オブジェクト当たり判定用の配列
	std::vector<Object3d*> objectsInfo_;

	Player* player_ = nullptr;

	std::unique_ptr<Model> boxModel_ = nullptr;
	std::unique_ptr<Object3d> box_[4];

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

