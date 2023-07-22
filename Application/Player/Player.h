#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
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

private:
	/// <summary>
	/// 移動の更新処理
	/// </summary>
	void MoveUpdate();

private:

	bool IsStickOn = false;

	float deadZone = 450;
	Vector3 moveValue = { 0.1f,0.0f,0.1f };

	// 接地フラグ
	bool onGround = true;
	// 落下ベクトル
	Vector3 fallV;
};

