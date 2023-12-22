#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
#include "GameCamera.h"
#include "PlayerBullet.h"
#include "Sprite.h"


class Player:public Object3d
{
public:
	// 3Dオブジェクト生成
	static Player* Create(Model* model = nullptr);

public:

	// デストラクタ
	~Player();

	// 初期化
	bool Initialize()override;

	// シルエット初期化
	void InitializeFlashLightRange();

	// 毎フレーム処理
	void Update()override;

	// 描画
	void Draw(ViewProjection* view)override;

	// プレイヤーの2D描画前面
	void Draw2DFront();

	// Imguiの描画
	void DrawImgui();

	// 懐中電灯の範囲オブジェの描画
	void DrawFlashLightRange(ViewProjection* view);

	// リセット関数
	void Reset();

public:// ゲッター

	// ポジションのゲッター
	const Vector3 GetWorldPosition ()const;

	// カメラ用のワールドトランスフォームの取得
	const WorldTransform *GetCameraWorld()const;

	// プレイヤー前方にあるワールドトランスフォームの取得
	const Vector3 GetFrontPos()const;

	// プレイヤーの前方のベクトルを取得
	const Vector3 GetFrontVec()const;

	// プレイヤーの生きているフラグの取得
	const bool GetIsAlive()const;

	std::list<std::unique_ptr<PlayerBullet>>& GetBullets();

public:// セッター

	// ワールドトランスフォームの情報をセット
	void SetWorldTransInfo(WorldTransform worldTrans);

	// ゲームカメラをセット
	void SetGameCamera(GameCamera* gameCamera);

	

public:// 当たり判定

	// 衝突時コールバック関数
	void OnCollision();

	// 押し戻し衝突判定コールバック関数
	void pushBackOnCol();

	// 前方にいる敵の体と当たった場合に移動速度を下げる
	void OnColDownSpeed();

	// 前方にいる敵の体と当たっていない場合に移動速度を上げる
	void OnColUpSpeed();

	/// <summary>
	/// 当たり判定ヒットポイント
	/// </summary>
	void OnColHitPoint();

private:// プライベート関数
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

	// アタックフラグ
	bool IsAttack_ = false;

	// 生きているかフラグ
	bool IsAlive_ = true;

	// 懐中電灯の範囲のオブジェクト
	Object3d flashLightRangeObj_;
	std::unique_ptr<Model> flashLightRangeModel_;

#pragma region プレイヤーのHP関連
	// プレイヤーのHP関連
	float playerHitPoint_ = 0;
	float playerHitPointMax_ = 100;

	// プレイヤーのHPのスプライト
	std::unique_ptr<Sprite> playerHpBar_ = nullptr;
	std::unique_ptr<Sprite> playerHpInside_ = nullptr;

	// プレイヤーのHpのテクスチャハンドル
	std::unique_ptr<Texture> textureHandleHpBar_ = nullptr;
	std::unique_ptr<Texture> textureHandleHpInside_ = nullptr;

	Vector2 pHpBarPos_ = { 20,50 };
	Vector2 pHpInsidePos_ = { 20,50 };

	Vector2 pHpBarSize_ = { 500,70 };
	Vector2 pHpInsideSize_ = { 500,70 };
#pragma endregion

};

