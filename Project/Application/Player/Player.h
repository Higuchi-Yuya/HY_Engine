#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
#include "GameCamera.h"
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

	// ワールドトランスフォームの情報をセット
	void SetWorldTransInfo(WorldTransform worldTrans);

	// ゲームカメラをセット
	void SetGameCamera(GameCamera* gameCamera);

private:
	/// <summary>
	/// 移動の更新処理
	/// </summary>
	void MoveUpdate();

private:

	bool IsStickOn = false;

	WorldTransform cameraWorld_;
	float deadZone = 450;
	Vector3 moveValue = { 0.1f,0.0f,0.1f };

	// 接地フラグ
	bool onGround = true;
	// 落下ベクトル
	Vector3 fallV;

	Vector3 frontVec_ = { 0,0,1 };

	// 移動ベクトル
	Vector3 moveVel_;
	float moveSpeed_ = 0.2f;

	// 借りてくるカメラ
	GameCamera *bGameCamera;
};

