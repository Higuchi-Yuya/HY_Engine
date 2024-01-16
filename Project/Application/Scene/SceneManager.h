#pragma once

#include "TitleScene.h"
#include "GameScene.h"
#include "GameClearScene.h"
#include "GameOverScene.h"
#include "BeatEffect.h"

class SceneManager
{
public:
	// シーンクラス
	enum Scene {
		Title,
		Game,
		GameClear,
		GameOver,
	};
	enum ObjsType {
		Normal,
		Doors,
		Ranterns,
		HiguLumiRanterns
	};

public:// メンバ関数

	// デストラクタ
	~SceneManager();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// Imguiの更新処理
	/// </summary>
	void ImguiUpdate();

	/// <summary>
	/// 背面スプライトの描画処理
	/// </summary>
	void Draw2DBack();

	/// <summary>
	/// 3Dオブジェクトの描画処理
	/// </summary>
	void Draw3D();

	/// <summary>
	/// パーティクルの描画処理
	/// </summary>
	void DrawParticle();

	/// <summary>
	/// 前面スプライトの描画処理
	/// </summary>
	void Draw2DFront();

	/// <summary>
	/// ブルームを掛けるオブジェクトの描画
	/// </summary>
	void DrawBloomObject();

	/// <summary>
	/// 高輝度抽出するオブジェクトの描画
	/// </summary>
	void DrawHighLumiObj();

public:// 静的メンバ関数

public:// セッター
	void SetDxComon(DirectXCommon* dxCommon) { dxCommon_ = dxCommon; }
	void SetBeatEffect(BeatEffect* beatEffect);

public:// ゲッター

private:// プレイべーとメンバ関数

	/// <summary>
	/// ローダーの初期化処理
	/// </summary>
	void InitLoader();

	/// <summary>
	/// 各シーンに必要な初期設定の処理
	/// </summary>
	void InitScenesSets();

	/// <summary>
	/// ライトの更新処理
	/// </summary>
	void LightUpdate();

	/// <summary>
	/// シーンチェンジの更新処理
	/// </summary>
	void SceneChageUpdate();

	/// <summary>
	/// リセット関数
	/// </summary>
	void Reset();

private:// メンバ変数

	// 借りてくるコモン
	DirectXCommon* dxCommon_ = nullptr;

	// 鼓動エフェクト
	BeatEffect* beatEffect_ = nullptr;

	// シーンの状態
	Scene scene_ = Title;

	// タイトルシーン
	std::unique_ptr<TitleScene> titleScene_;

	// ゲームシーン
	std::unique_ptr<GameScene> gameScene_;

	// ゲームクリアシーン
	std::unique_ptr<GameClearScene> gameClearScene_;

	// ゲームオーバーシーン
	std::unique_ptr<GameOverScene> gameOverScene_;

	#pragma region 光源関連
	// オブジェクト共通のライトの初期化
	std::unique_ptr<LightGroup> light = nullptr;

	// 平行光源
	bool isActiveDirectional = false;

	// 点光源のステータス
	float pointLightPos[3] = { 0,5,0 };
	Vector3 pointLightColor = { 0.92f,0.684f,0.56f };
	Vector3 pointLightAtten = { 0.12f,0.1f,0.02f };
	bool isActivePoint = false;
	float pointLightIndensity = 1.2f;
	float pointLightRadius = 70;
	float pointLightDecay = 15.5f;
	float pointLightDistance = 10;

	// スポットライトのステータス
	Vector3 spotLightDir = { 0,0,0 };
	Vector3 spotLightPos = { 0,50,0 };
	Vector3 spotLightColor = { 1,1,1 };
	Vector3 spotLightAtten = { 0.1f,0.1f,0.05f };
	Vector2 spotLightFactorAngle = { 10.0f,30.0f };
	
	bool isActiveSpot = true;

	// 丸影のステータス
	Vector3 circleShadowCasterPos = { 0,0,0 };
	Vector3 circleShadowDir = { 0,-1,0 };
	Vector3 circleShadowAtten = { 6.5f,0.6f,0.0f };
	Vector2 circleShadowFactorAngle = { 0.0f,0.5f };
	bool isActiveCircleShadow = false;

	Vector3 AmColor = { 1,0,0 };
	Vector3 DiColor = { 0,1,0 };
	Vector3 SpColor = { 0,0,1 };

	// フォグ
	std::unique_ptr<Fog> fog = nullptr;
	bool isFogActive = true;
	#pragma endregion

	#pragma region モデル

	std::unique_ptr<Model> modelMedama_ = nullptr;

	std::unique_ptr<Model> modelSkydome_ = nullptr;
	std::unique_ptr<Model> modelGround_ = nullptr;

	// 木
	std::unique_ptr<Model> modelTreeBald_ = nullptr;
	std::unique_ptr<Model> modelTreeNormal_ = nullptr;

	// フェンス
	std::unique_ptr<Model> modelFence_ = nullptr;
	std::unique_ptr<Model> modelFencePost_ = nullptr;
	std::unique_ptr<Model> modelLatticeDoor_ = nullptr;

	// お墓
	std::unique_ptr<Model> modelGraveCross = nullptr;
	std::unique_ptr<Model> modelGraveSquare = nullptr;

	// ランタン
	std::unique_ptr<Model> modelWallRantern_ = nullptr;

	// 街灯
	std::unique_ptr<Model> modelPostRantern_ = nullptr;

	// 壁
	std::unique_ptr<Model> modelWall02_ = nullptr;
	std::unique_ptr<Model> modelWall03_ = nullptr;

	// 椅子
	std::unique_ptr<Model> modelChair_ = nullptr;

	// 十字架
	std::unique_ptr<Model> modelCross_ = nullptr;
	std::unique_ptr<Model> modelStoneCross_ = nullptr;

	#pragma endregion

	#pragma region カメラ関連
	// ビュープロジェクション
	std::unique_ptr<GameCamera>gameCamera;
#pragma endregion

	#pragma region シーンチェンジ関連

	// フェードインフェードアウト用の画像ハンドル
	std::unique_ptr<Texture> blackOutTexHandle_;

	// フェードインフェードアウト用の画像スプライト
	std::unique_ptr<Sprite> blackOut = nullptr;

	// ブラックアウトの変数
	float blackAlpha = 0.0f;
	bool sceneChangeFlag = false;
	Scene oldScene = Scene::Title;

	#pragma endregion

	#pragma region レベルデータ関連

	// レベルデータ
	std::unique_ptr<LevelData> levelData_;

	// レベルデータに登録するモデルの配列
	std::map<std::string, Model*> models;

	// レベルデータに登録するオブジェクトの配列
	std::vector<Object3d*> objects;

	// お墓のドアの配列
	std::vector<Object3d*> latticeDoors_;

	// ランタンの配列
	std::vector<Object3d*> ranterns_;

	// ランタンの高輝度用配列
	std::vector<Object3d*> highLumiRanterns_;

	// ポイントライト情報の配列
	std::vector<WorldTransform*> pointLightsInfo_;

	#pragma endregion

	#pragma region 当たり判定
	std::unique_ptr<GameCollider> gameCollider;
	#pragma endregion

};

