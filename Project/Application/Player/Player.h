#pragma once
#include "Object3d.h"
#include "ImGuiManager.h"
#include "GameCamera.h"
#include "PlayerBullet.h"
#include "Sprite.h"
#include "BillboardTex.h"

// 前方宣言
class ItemPaper;

class Player:public Object3d
{
private:
	enum 
	{
		ZERO,
		ONE,
	};
	
	enum firstEventState
	{
		FirstMove,	// 最初の移動
		InterTurn,	// 補間の回転
		FirstTurn,	// 一回目の回転
		SecondTurn,	// 二回目の回転
		Surprised,	// びっくりのフェーズ
	};

	enum ringEaseState
	{
		ValueSet,
		EaseRing,
	};

public:
	// 3Dオブジェクト生成
	static Player* Create(Model* model = nullptr);

public:

	// デストラクタ
	~Player();

	// 初期化
	bool Initialize()override;

	// シルエット初期化
	void InitializeSearchRing();

	// ゲームシーンの最初のイベントの更新処理
	void gameSceneFirstUpdate();

	// 毎フレーム処理
	void Update()override;

	/// <summary>
	/// サーチリングの更新処理
	/// </summary>
	void SearchRingUpdate(std::vector<ItemPaper*>itemPapers);

	// 描画
	void Draw(ViewProjection* view)override;

	// リングの描画
	void DrawRing(ViewProjection* view);

	// 描画ビルボード
	void DrawBillTex();

	// プレイヤーの2D描画前面
	void Draw2DFront();

	// Imguiの描画
	void DrawImgui();

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

	// プレイヤーのきょろきょろが終了しているかどうか
	const bool GetIsEndTurnAround()const;

	// フラッシュが有効かどうかを取得
	const bool GetIsFlash()const;

	// フラッシュの最後のカウントになっているかどうかを取得
	const bool GetIsFlashMax()const;

public:// セッター

	// ワールドトランスフォームの情報をセット
	void SetWorldTransInfo(WorldTransform worldTrans);

	// ゲームカメラをセット
	void SetGameCamera(GameCamera* gameCamera);

	/// <summary>
	/// アイテムの範囲なのかを設定
	/// </summary>
	/// <param name="isItemRange"></param>
	void SetIsItemRange(bool isItemRange);

	/// <summary>
	/// アイテムのとれる範囲なのかを設定
	/// </summary>
	/// <param name="isItemInRange"></param>
	void SetIsItemInRange(bool isItemInRange);

	/// <summary>
	/// 移動がかのうかどうかを設定
	/// </summary>
	/// <param name="isCamMove"></param>
	void SetIsCanMove(bool isCamMove);

	/// <summary>
	/// フラッシュのカウントが最大かどうかを設定
	/// </summary>
	/// <param name="isFlashMax"></param>
	void SetIsFlashMax(bool isFlashMax);

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

	/// <summary>
	/// フラッシュの更新処理
	/// </summary>
	void FlashUpdate();

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

	// サーチリングのオブジェクト
	Object3d searchRingObj_;
	std::unique_ptr<Model> searchRingModel_;

	// リングのオブジェクト
	Object3d ringObj_;
	std::unique_ptr<Model> ringModel_;

	Vector3 defuRingScale_ = { 2,1,2 };
	Vector3 bigRingScale_ = { 5,1,5 };

	float defuRingAlpha_ = 0.6f;
	float bigRingAlpha_ = 0;

	Easing easeRing_;

	ringEaseState ringState_;

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

#pragma region ゲームシーン最初の処理関連
	// きょろきょろの回数
	uint32_t turnAroundCount_ = 0;

	// ビルボードのテクスチャ
	BillboardTex questionBillTex_;

	std::unique_ptr<Texture> questionTex_ = nullptr;
	std::unique_ptr<Texture> surprisedTex_ = nullptr;

	// アイテムの範囲なのか
	bool IsItemRange_ = false;
	bool IsItemInRange_ = false;

	// はてなのイージング
	Easing easeQuestion_;

	// うごく角度
	const float easeQ_Rota1 = 25.0f;
	const float easeQ_Rota2 = -25.0f;

	const float easeSRota = 180.0f;

	// 回転のフラグ
	bool IsEndTurnAround_ = false;

	// 移動がかのうかどうかのフラグ
	bool IsCanMove_ = false;
	
	firstEventState firstEventState_ = FirstMove;

	uint32_t aroundStopTimer = 0;
	uint32_t aroundStopTimeLimit = 20;

	float firstEventMoveZS = -42;
	float firstEventMoveZE = -32;


#pragma endregion

#pragma region 懐中電灯のフラッシュ関連

	// フラッシュが押されていたら
	bool IsFlash_ = false;

	// フラッシュライトがマックスになったら
	bool IsFlashMax_ = false;

#pragma endregion

};

