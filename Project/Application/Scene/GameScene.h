#pragma once
#include "Sprite.h"
#include "Sound.h"
#include "Object3d.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Quaternion.h"
#include "LightGroup.h"
#include "ImGuiManager.h"
#include "CollisionPrimitive.h"
#include "LevelLoder.h"
#include "TextureManager.h"
#include "Player.h"
#include "Enemy.h"
#include "Dissolve.h"
#include "ParticleManager.h"

#include "TimerUI.h"
#include "OperationUI.h"
#include "GameCamera.h"
#include "GameCollider.h"

#include <vector>
#include <BillboardTex.h>
#include "ItemPaper.h"
#include "BeatEffect.h"

class GameScene
{
public:
	enum ObjsType {
		Normal,
		Doors,
		Ranterns,
		HiguLumiRanterns
	};
public:// メンバ関数
	
	// デストラクタ
	~GameScene();

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// Imguiの更新処理
	void ImguiUpdate();

	// 描画処理
	void Draw2DBack();

	void Draw3D();

	void DrawParticle();

	void Draw2DFront();

	/// <summary>
	/// ブルームを掛けるオブジェクトの描画
	/// </summary>
	void DrawBloomObject();

	/// <summary>
	/// 高輝度抽出するオブジェクトの描画
	/// </summary>
	void DrawHighLumiObj();

	/// <summary>
	/// ステンシルの読み込みをするオブジェクトの描画
	/// </summary>
	void DrawTransParentObj();

	/// <summary>
	/// ステンシルの書き込みをするオブジェクトの描画
	/// </summary>
	void DrawShieldObj();

	/// <summary>
	/// ビルボードのオブジェの描画
	/// </summary>
	void DrawBillboardTex();

	// リセット関数
	void Reset();

	// 敵のレベルデータを読み込む
	void LoadEnemy();

public:// セッター
	/// <summary>
	/// コマンドリストの設定
	/// </summary>
	/// <param name="dxCommon"></param>
	void SetCmdList(ID3D12GraphicsCommandList* cmdList) { commandList_ = cmdList; }

	/// <summary>
	/// ゲームカメラを設定
	/// </summary>
	/// <param name="gameCamera"></param>
	void SetGameCamera(GameCamera* gameCamera) { gameCamera_ = gameCamera; }

	/// <summary>
	/// ゲームコライダーを設定
	/// </summary>
	/// <param name="gameCollider">コライダーのデータ</param>
	void SetGameCollider(GameCollider* gameCollider) { gameCollider_ = gameCollider; }

	/// <summary>
	/// モデルデータ配列を設定
	/// </summary>
	/// <param name="models">モデルデータ</param>
	void SetModels(std::map<std::string, Model*> models) { models_ = models; }

	/// <summary>
	/// オブジェクト配列を設定
	/// </summary>
	/// <param name="objs">オブジェクトの配列</param>
	/// <param name="objType">配列のタイプ</param>
	void SetObjs(std::vector<Object3d*> objs,ObjsType objType);

	/// <summary>
	/// ライトグループを設定
	/// </summary>
	/// <param name="lightGroup">ライトグループ</param>
	void SetLightGroup(LightGroup* lightGroup) { lightGroup_ = lightGroup; }

	/// <summary>
	/// ポイントライトの情報を設定
	/// </summary>
	/// <param name="pointLightsInfo">ポイントライトの情報が格納されている配列</param>
	void SetPointInfo(std::vector<WorldTransform*> pointLightsInfo) { pointLightsInfo_ = pointLightsInfo; }

	/// <summary>
	/// 鼓動エフェクトを設定
	/// </summary>
	/// <param name="beatEffect"></param>
	void SetBeatEffect(BeatEffect* beatEffect) { beatEffect_ = beatEffect; }

public:// ゲッター

	Player* GetPlayer() { return player_.get(); }

	/// <summary>
	/// シーン終了しているかを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsSceneFinsh() { return IsSceneFinsh_; }

	/// <summary>
	/// ゲームをクリアしているかを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsGameClear() { return IsGameClear_; }

private:// サブクラス

	// 敵のウェーブ
	enum EnemyWave {
		wave01,
		wave02,
		wave03,
	};

private:// プライベート関数

	/// <summary>
	/// ゲームシーンの更新処理
	/// </summary>
	void GameSceneUpdate();

	/// <summary>
	/// 光源の更新処理
	/// </summary>
	void LightUpdate();

	/// <summary>
	/// エネミーのゲームシーンの更新処理
	/// </summary>
	void EnemyGameUpdate();

	/// <summary>
	/// アイテムの更新処理
	/// </summary>
	void ItemUpdate();

	/// <summary>
	/// 最初のイベントの更新処理
	/// </summary>
	void FirstEventUpdate();

private:// メンバ変数

	// 入力
	std::unique_ptr<Input> input_ = nullptr;

	// 借りてくるコマンドリスト
	ID3D12GraphicsCommandList* commandList_ = nullptr;

	//	借りてくるライトグループ
	LightGroup* lightGroup_;

	// ポイントライト情報の配列
	std::vector<WorldTransform*> pointLightsInfo_;

	// シーン終了フラグ
	bool IsSceneFinsh_;

	// ゲームクリアしてるかフラグ
	bool IsGameClear_;

	float spotDownPosY = 0.4f;
	Vector3 circleShadowCasterPos = { 0,0,0 };
#pragma region オブジェクト関連
	bool isDissolve = false;
	std::unique_ptr<Model> playerModel_ = nullptr;
#pragma endregion

#pragma region 音関連
	Sound sound;
	bool isActiveSound = false;
	bool isStopSound = false;
#pragma endregion

#pragma region プレイヤー関連
	std::unique_ptr<Player> player_;

	// プレイヤーのポイントライトの輝度
	float playerPointLightIndensity = 0.8f;
	// プレイヤーのポイントライトの範囲
	float playerPointLightRadius_ = 2;
	// プレイヤーのポイントライトの減衰値
	float playerPointLightDecay_ = 1;
#pragma endregion

#pragma region 当たり判定関連
	GameCollider* gameCollider_ = nullptr;
#pragma endregion

#pragma region カメラ関連
	GameCamera* gameCamera_ = nullptr;
#pragma endregion

#pragma region エネミー関連
	std::vector<Enemy*>enemys_;
#pragma endregion

#pragma region 敵のウェーブ関連

	// エネミーのウエーブを管理するもの
	EnemyWave enemyWave_ = EnemyWave::wave01;

	// ゲームのウェーブごとの時間
	float waveTimer_ = 0;
	const float waveTimeMax_ = 60 * 30;

	// 今のウェーブ
	uint32_t waveTimeNum_ = 0;

	// エネミーのスポーン時間
	float enemySpawnTimer_ = 0;

	// エネミーのウェーブごとのスポーン間隔
	const float enemySpawnTimeMax1_ = 60 * 5;
	const float enemySpawnTimeMax2_ = 60 * 4;
	const float enemySpawnTimeMax3_ = 60 * 3;

	// 抽選するエネミーのスポーンパターン
	int randomWave01_ = 0;

	// ウェーブが終わった時の判別フラグ
	bool IsEndWave = false;

#pragma endregion

#pragma region UI関連
	std::unique_ptr<TimerUI>timerUi_;

	std::unique_ptr<OperationUI> operationUi_;
#pragma endregion

#pragma region ローダー関連
	// レベルデータ
	std::unique_ptr<LevelData> levelData_;

#pragma endregion

#pragma region 借りてくるオブジェの配列

	// レベルデータに登録するモデルの配列
	std::map<std::string, Model*> models_;

	// オブジェクトの配列(借りてくる)
	std::vector<Object3d*> objects_;

	// お墓のドアの配列(借りてくる)
	std::vector<Object3d*> latticeDoors_;

	// ランタンの配列(借りてくる)
	std::vector<Object3d*> ranterns_;

	// ランタンの高輝度用配列
	std::vector<Object3d*> highLumiRanterns_;
#pragma endregion

#pragma region アイテム関連
	std::vector<ItemPaper*>itemPapers_;

	// 表示されているかどうか
	bool IsItemDisplay_ = false;

	float doorRotYValue = 0.5f;
	float door1Roty = -160;
	float door2Roty = 160;

	Easing easeItem1;
	Easing easeItem2;
	Easing easeItem3;

	Vector2 easeItemPos1 = { 100,150 };
	Vector2 easeItemPos2 = { 180,150 };
	Vector2 easeItemPos3 = { 260,150 };

	// 三つのアイテムが全て集まったら
	
#pragma endregion

#pragma region 最初のイベントシーン関連
	float doorLDefuRota = -90;
	float doorRDefuRota = 90;

	float doorRotaValue = 75;
	Easing easeDoorRota_;
	bool IsDoorFirstTurn = false;
#pragma endregion


	ItemPaper testItem_;

	// 鼓動エフェクト
	BeatEffect* beatEffect_ = nullptr;
};

