#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
#include "Player.h"
#include <ParticleManager.h>
#include "IEnemyState.h"
#include "EnemyHeart.h"

class Enemy:public Object3d
{
public:// サブクラス
	enum class StateType
	{
		Spawn,// 生成
		Alive,// 生きているとき
		Dead,// 死んでる時
	};

public:
	~Enemy();

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

	/// <summary>
	/// フォワードレンダリングオブジェクトの描画
	/// </summary>
	/// <param name="view">ゲームカメラ</param>
	void DrawForward3D(ViewProjection* view);

	// パーティクル描画
	void DrawParticle(ViewProjection* view);

	// 衝突時コールバック関数
	void OnCollision();

	// 押し戻し衝突判定コールバック関数
	void pushBackOnCol();

	// 戻るフェーズ関連をリセット
	void ResetBack();

	/// <summary>
	/// プレイヤーが近い時にでるパーティクルの更新処理
	/// </summary>
	void NearPlayerParticleUpdate();

	/// <summary>
	/// ガイドライトのカウントをプラス
	/// </summary>
	void AddLightMoveCount();

public:// ゲッター

	// 生きているかを取得
	const bool GetAlive() { return curenntState_->GetIsAlive(); }

	// 死亡モーションが全て終わっているかを取得
	const bool GetDeadMotionEnd() { return curenntState_->GetIsDeadMotionEnd(); }

	// ディゾルブ処理が終了しているかどうかを取得
	const bool GetDissolveEnd() { return curenntState_->GetIsDissolveEnd(); }

	// 今の状態を取得
	const StateType GetState() { return nowState_; }

	// 生きているときの今の状態を取得
	const IEnemyState::AliveState GetAliveState() { return curenntState_->GetAliveState(); }

	// ガイドライトの移動完了カウントを取得
	const int GetLightMoveCount() { return lightMoveCount; }

	// 死亡時の番号を取得
	const int GetDeadNum() { return deadNum_; }

public:// セッター
	// ワールドトランスフォームの情報をセット
	void SetWorldTransInfo(WorldTransform worldTrans);

	// ポジションのセット
	void SetWorldPos(Vector3 pos);

	/// <summary>
	/// プレイヤーを設定
	/// </summary>
	/// <param name="player">プレイヤーのポインタ</param>
	static void SetPlayer(Player* player);

	/// <summary>
	/// いきているかどうかを設定
	/// </summary>
	/// <param name="isAlive"></param>
	void SetIsAlive(const bool isAlive);

	/// <summary>
	/// 敵の生きているときの行動状態を設定
	/// </summary>
	/// <param name="aState">状態</param>
	void SetAliveState(IEnemyState::AliveState aState);

	/// <summary>
	/// 敵の死亡モーションが終了しているかどうかを設定
	/// </summary>
	/// <param name="isDeadMotionEnd"></param>
	void SetDeadMotionEnd(const bool isDeadMotionEnd);

	/// <summary>
	/// ライトの移動処理が終了しているかを設定
	/// </summary>
	/// <param name="isLightMoveEnd"></param>
	void SetLightMoveEnd(const bool isLightMoveEnd);

	/// <summary>
	/// 一度だけ死亡時の番号を設定
	/// </summary>
	/// <param name="deadNum"></param>
	void SetDeadNum(const int deadNum);

	/// <summary>
	/// 状態を変更
	/// </summary>
	/// <param name="newState">新しい状態</param>
	void ChageState(StateType stateType);

public:
	// 衝突点
	Vector3 interPos;
	// 排斥ベクトル
	Vector3 rejectVec;

private:// 静的メンバ変数

	// デフォルトのモデル
	static std::unique_ptr<Model> modelDefu_;

private:
	// 当たったかどうか
	bool IsCollision = false;

	// フラグ
	bool isH = false;

	// 現在の状態
	StateType nowState_ = StateType::Spawn;

	// ライフ
	uint32_t nowLife_ = 0;
	const uint32_t maxLife_ = 3;

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
	int particleNum = 10;

#pragma endregion

	int lightMoveCount = 0;

	int deadNum_ = 0;
	bool IsSetDeadNum_ = false;
	bool IsLightMoveEnd_ = false;

	// こころ
	std::unique_ptr<EnemyHeart> heart_;

	// 今の状態
	IEnemyState* curenntState_;
};

