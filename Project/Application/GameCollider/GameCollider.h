#pragma once
#include "Enemy.h"
#include "Player.h"
#include "ParticleManager.h"
#include "Collision.h"
#include <ItemPaper.h>

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
	void Update();

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
	/// 球の情報をベクター型に格納
	/// </summary>
	/// <param name="sphere">球の情報</param>
	void AddObjSphere(Sphere sphere);

	/// <summary>
	/// ボックスの情報をベクター型に格納
	/// </summary>
	/// <param name="box">ボックスの情報</param>
	void AddObjBox(Box box);

	/// <summary>
	/// アイテムの情報をベクター型に格納
	/// </summary>
	/// <param name="item"></param>
	void AddItem(ItemPaper* item);

	/// <summary>
	/// プレイヤーを取得する
	/// </summary>
	/// <param name="player">プレイヤー</param>
	void SetPlayer(Player* player);

	/// <summary>
	/// リセット関数
	/// </summary>
	void Reset();

	/// <summary>
	/// クリア範囲フラグを取得
	/// </summary>
	const bool GetIsClear() { return IsClear_; };

private:// プライベート関数
	// エネミー関連の当たり判定処理
	void EnemyCollisionUpdate();

	// プレイヤー周りの当たり判定処理
	void PlayerCollisionUpdate();

	// オブジェクト周りとの当たり判定処理
	void ObjAronudCollisionUpdate();

	// アイテム周りとの当たり判定処理
	void ItemCollisionUpdate();

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

	// 球の当たり判定用の配列
	std::vector<Sphere> objSpheres_;
	// ボックスの当たり判定用の配列
	std::vector<Box>objBox_;

	// アイテムの当たり判定用の配列
	std::vector<ItemPaper*> ItemPapersInfo_;

	Player* player_ = nullptr;

	std::unique_ptr<Model> boxModel_ = nullptr;
	std::unique_ptr<Object3d> box_[4];

	// ドアが開いた時のクリア判定
	std::unique_ptr<Object3d> testBox_;
	// ドアが開いた時のクリア判定フラグ
	bool IsClear_ = false;

	// プレイヤーの情報を格納するスフィア
	Sphere pcol_;


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

