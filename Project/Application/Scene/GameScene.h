#pragma once
#include "Sprite.h"
#include "Sound.h"
#include "Object3d.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Quaternion.h"
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
#include "IScene.h"
#include "GameLight.h"
#include "EnemyHeart.h"
class GameScene:public IScene
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
	void Initialize()override;

	// 更新処理
	void Update()override;

	// Imguiの更新処理
	void ImguiUpdate()override;

	// 描画処理
	void Draw2DBack();

	/// <summary>
	/// フォワード3Dオブジェクトの描画処理
	/// </summary>
	void DrawForward3D()override;

	/// <summary>
	/// ディファード3Dオブジェクトの描画処理
	/// </summary>
	void DrawDefrred3D()override;

	void DrawParticle();

	void Draw2DFront()override;

	/// <summary>
	/// ブルームを掛けるオブジェクトの描画
	/// </summary>
	void DrawBloomObject()override;

	/// <summary>
	/// 高輝度抽出するオブジェクトの描画
	/// </summary>
	void DrawHighLumiObj()override;

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
	void Reset()override;

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
	void SetObjs(std::vector<DeferredObject3d*> objs,ObjsType objType);

	/// <summary>
	/// ブルームオブジェクトを設定
	/// </summary>
	/// <param name="objs"></param>
	void SetBloomObjs(std::vector<Object3d*>objs);

	/// <summary>
	/// 鼓動エフェクトを設定
	/// </summary>
	/// <param name="beatEffect"></param>
	void SetBeatEffect(BeatEffect* beatEffect);

	/// <summary>
	/// ライトグループを設定
	/// </summary>
	/// <param name="lightGroup">ライトグループ</param>
	void SetLightGroup(LightGroup* lightGroup);

	/// <summary>
	/// ポイントライトの情報を設定
	/// </summary>
	/// <param name="pointLightsInfo">ポイントライトの情報が格納されている配列</param>
	void SetPointInfo(std::vector<WorldTransform*> pointLightsInfo);

public:// ゲッター

	Player* GetPlayer() { return player_.get(); }

	/// <summary>
	/// シーン終了しているかを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsSceneFinsh()override { return IsSceneFinsh_; }

	/// <summary>
	/// ゲームをクリアしているかを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsGameClear()override { return IsGameClear_; }

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

	// シーン終了フラグ
	bool IsSceneFinsh_;

	// ゲームクリアしてるかフラグ
	bool IsGameClear_;

	
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
	std::vector<DeferredObject3d*> objects_;

	// お墓のドアの配列(借りてくる)
	std::vector<DeferredObject3d*> latticeDoors_;

	// ランタンの配列(借りてくる)
	std::vector<DeferredObject3d*> ranterns_;

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

	// 鼓動エフェクト
	BeatEffect* beatEffect_ = nullptr;

	// ヒントアイテムの表示関連
	std::unique_ptr<Texture> tipsDisplayTex_;
	std::unique_ptr<Sprite> tipsDisplaySprite_;

	Vector2 tipsDisplayPos_ = { 1135,220 };
	Vector2 tipsDisplaySize_ = { 200,116 };

	bool IsTipsDisplay = false;

	// ゲームライト
	std::unique_ptr<GameLight> gameLight_;
};

