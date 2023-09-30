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
#include "FbxLoader.h"
#include "FbxAnimetion.h"
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

class CollisionManager;
class Player;
class TouchableObject;

class GameScene
{
public:// メンバ関数
	
	// デストラクタ
	~GameScene();

	// 初期化処理
	void Initialize();

	// 更新処理
	void Update();

	// Imguiの更新処理
	void ImguiUpdate();

	void SetDxComon(DirectXCommon* dxCommon) { dxCommon_ = dxCommon; }

	// 描画処理
	void Draw2DBack();

	void Draw3D();

	void DrawParticle();

	void Draw2DFront();

	// リセット関数
	void Reset();

	// 敵のレベルデータを読み込む
	void LoadEnemy();

private:// サブクラス
	// シーンクラス（仮）
	enum class Scene {
		Title,
		Game,
		Result,
	};

	// 敵のウェーブ
	enum EnemyWave {
		wave01,
		wave02,
		wave03,
	};

private:// プライベート関数
	/// <summary>
	/// タイトルシーンの更新処理
	/// </summary>
	void TitleUpdate();

	/// <summary>
	/// ゲームシーンの更新処理
	/// </summary>
	void GameSceneUpdate();

	/// <summary>
	/// リザルトシーンの更新処理
	/// </summary>
	void ResultSceneUpdate();

	/// <summary>
	/// シーンチェンジの更新処理
	/// </summary>
	void SceneChageUpdate();

	/// <summary>
	/// エネミーのゲームシーンの更新処理
	/// </summary>
	void EnemyGameUpdate();

private:// メンバ変数

	// 入力
	std::unique_ptr<Input> input_ = nullptr;

#pragma region 光源関連
	// オブジェクト共通のライトの初期化
	std::unique_ptr<LightGroup> light = nullptr;

	// 平行光源
	bool isActiveDirectional = false;

	// 点光源のステータス
	float pointLightPos[3] = { 0,5,0 };
	float pointLightColor[3] = { 1,1,1 };
	float pointLightAtten[3] = { 0.0f,0.0f,0.001f };
	bool isActivePoint = false;

	// スポットライトのステータス
	Vector3 spotLightDir = { 0,-1,0 };
	Vector3 spotLightPos = { 0,50,0 };
	Vector3 spotLightColor = { 1,1,1 };
	Vector3 spotLightAtten = { 0.0f,0.001f,0.001f };
	Vector2 spotLightFactorAngle = { 20.0f,90.0f };
	bool isActiveSpot = true;

	// 丸影のステータス
	Vector3 circleShadowCasterPos = { 0,0,0 };
	Vector3 circleShadowDir = { 0,-1,0 };
	Vector3 circleShadowAtten = { 1.5f,0.6f,0.0f };
	Vector2 circleShadowFactorAngle = { 0.0f,0.5f };
	bool isActiveCircleShadow = true;

	Vector3 AmColor = { 1,0,0 };
	Vector3 DiColor = { 0,1,0 };
	Vector3 SpColor = { 0,0,1 };

	// フォグ
	std::unique_ptr<Fog> fog = nullptr;
	bool isFogActive = true;
#pragma endregion


#pragma region テクスチャハンドル

	std::unique_ptr<Texture> textureHandleDefu;
	std::unique_ptr<Texture> titleFontTexHandle;
	std::unique_ptr<Texture> titleBackTexHandle;
	std::unique_ptr<Texture> titleButtonTexHandle;

	// フェードインフェードアウト用の画像ハンドル
	std::unique_ptr<Texture> blackOutTexHandle_;
#pragma endregion


#pragma region スプライト

	std::unique_ptr<Sprite> spriteProvisional = nullptr;
	std::unique_ptr<Sprite> titleKariFont = nullptr;
	std::unique_ptr<Sprite> titleKariBack = nullptr;
	std::unique_ptr<Sprite> titleKariPressA = nullptr;
	Vector2 spritePos;

	// フェードインフェードアウト用の画像スプライト
	std::unique_ptr<Sprite> blackOut = nullptr;
#pragma endregion


#pragma region モデル

	std::unique_ptr<Model> playerModel_ = nullptr;
	std::unique_ptr<Model> modelMedama_ = nullptr;

	std::unique_ptr<Model> modelSkydome_ = nullptr;
	std::unique_ptr<Model> modelGround_ = nullptr;

	// 木
	std::unique_ptr<Model> modelTreeBald_ = nullptr;
	std::unique_ptr<Model> modelTreeNormal_ = nullptr;

	// フェンス
	std::unique_ptr<Model> modelFence_ = nullptr;
	std::unique_ptr<Model> modelFencePost_ = nullptr;

	// お墓
	std::unique_ptr<Model> modelGraveCross = nullptr;
	std::unique_ptr<Model> modelGraveSquare = nullptr;

#pragma endregion


#pragma region オブジェクト関連
	bool isDissolve = false;
#pragma endregion


#pragma region カメラ関連
	// ビュープロジェクション
	std::unique_ptr<GameCamera>gameCamera;
#pragma endregion


#pragma region 音関連
	Sound sound;
	bool isActiveSound = false;
	bool isStopSound = false;
#pragma endregion


#pragma region 当たり判定
	std::unique_ptr < GameCollider> gameCollider;
#pragma endregion


#pragma region FBXモデルの確認
	//std::unique_ptr<FbxModel> fbxmodel_;
	WorldTransform fbxTrans_;
	//std::unique_ptr<FbxAnimetion> modelAnim_;
	DirectXCommon* dxCommon_ = nullptr;

	float frem = 0;
	uint32_t BoneNum = 0;
#pragma endregion


#pragma region プレイヤー関連
	std::unique_ptr<Player> player_;

#pragma endregion


#pragma region エネミー関連
	std::vector<Enemy*>enemys_;
#pragma endregion


#pragma region シーンチェンジ関連
	// シーン管理
	Scene scene = Scene::Title;

	// ブラックアウトの変数
	float blackAlpha = 0.0f;
	bool sceneChangeFlag = false;
	Scene oldScene = Scene::Title;
	bool resultChange = false;
#pragma endregion


#pragma region レベルデータ関連
	// レベルデータ
	std::unique_ptr<LevelData> levelData_;

	// レベルデータに登録するモデルの配列
	std::map<std::string, Model*> models;
	// レベルデータに登録するオブジェクトの配列
	std::vector<Object3d*> objects;

#pragma endregion


#pragma region 敵のウェーブ関連

	// エネミーのウエーブを管理するもの
	EnemyWave enemyWave_ = EnemyWave::wave01;

	// ゲームのウェーブごとの時間
	float waveTimer_ = 0;
	const float waveTimeMax_ = 120 * 60 / 3;

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

};

