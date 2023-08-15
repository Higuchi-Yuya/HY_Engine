#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
#include "GameCamera.h"
#include "PlayerBullet.h"
class Player:public Object3d
{
public:
	// 3Dオブジェクト生成
	static Player* Create(Model* model = nullptr);

public:
	~Player();
	// 初期化
	bool Initialize()override;

	// 毎フレーム処理
	void Update()override;

	// 描画
	void Draw(ViewProjection* view)override;

	// 衝突時コールバック関数
	void OnCollision(const CollisionInfo& info);

	// ポジションのゲッター
	const Vector3 GetWorldPosition ()const;

	// カメラ用のワールドトランスフォームの取得
	const WorldTransform *GetCameraWorld()const;

	// プレイヤー前方にあるワールドトランスフォームの取得
	const Vector3 GetFrontPos()const;

	// ワールドトランスフォームの情報をセット
	void SetWorldTransInfo(WorldTransform worldTrans);

	// ゲームカメラをセット
	void SetGameCamera(GameCamera* gameCamera);

	const std::list<std::unique_ptr<PlayerBullet>>& GetBullets();

	// 押し戻し衝突判定コールバック関数
	void pushBackOnCol();

	// 前方にいる敵の体と当たった場合に移動速度を下げる
	void OnColDownSpeed();

	// 前方にいる敵の体と当たっていない場合に移動速度を上げる
	void OnColUpSpeed();

private:
	/// <summary>
	/// 移動の更新処理
	/// </summary>
	void MoveUpdate();

	/// <summary>
	/// 攻撃処理
	/// </summary>
	void Attack();

public:// パブリック変数
	// 衝突点
	Vector3 interPos;
	// 排斥ベクトル
	Vector3 rejectVec;


private:

	bool IsStickOn = false;

	WorldTransform cameraWorld_;
	float deadZone = 450;
	Vector3 moveValue = { 0.1f,0.0f,0.1f };

	//プレイヤー関連
	std::list<std::unique_ptr<PlayerBullet>> bullets_;

	// 接地フラグ
	bool onGround = true;
	// 落下ベクトル
	Vector3 fallV;

	Vector3 frontVec_ = { 0,0,1 };

	// 移動ベクトル
	Vector3 moveVel_;
	float moveSpeed_ = 0.2f;
	float moveSpeedMax_ = 0.2f;
	float moveSpeedMin_ = 0.05f;

	// 借りてくるカメラ
	GameCamera *bGameCamera;

	// 弾関連
	// 前にある仮のワールドトランスフォーム
	WorldTransform frontW_;

	//モデル
	std::unique_ptr<Model> bulletModel_ = nullptr;

	// 弾のインターバル
	float bulletInterval = 20;
	float bulletInterTimer = 0;

};

