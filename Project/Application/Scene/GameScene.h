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

private:// サブクラス
	// シーンクラス（仮）
	enum class Scene {
		Title,
		Game,
		Result,
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
	/// シーンチェンジの更新処理
	/// </summary>
	void SceneChageUpdate();

private:// メンバ変数

	// 入力
	std::unique_ptr<Input> input_ = nullptr;

#pragma region 光源関連
	// オブジェクト共通のライトの初期化
	std::unique_ptr<LightGroup> light = nullptr;

	// 平行光源
	bool isActiveDirectional = true;

	// 点光源のステータス
	float pointLightPos[3] = { 0,1,0 };
	float pointLightColor[3] = { 1,1,1 };
	float pointLightAtten[3] = { 0.3f,0.1f,0.1f };
	bool isActivePoint = false;

	// スポットライトのステータス
	Vector3 spotLightDir = { 0,-1,0 };
	Vector3 spotLightPos = { 0,5,0 };
	Vector3 spotLightColor = { 1,1,1 };
	Vector3 spotLightAtten = { 0.0f,0.0f,0.0f };
	Vector2 spotLightFactorAngle = { 20.0f,30.0f };
	bool isActiveSpot = false;

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

	std::unique_ptr<Model> modelSkydome = nullptr;
	std::unique_ptr<Model> modelGround = nullptr;

#pragma endregion


#pragma region オブジェクト関連
	std::unique_ptr<Object3d> objSkydome = nullptr;
	std::unique_ptr<Object3d> objGround = nullptr;

	std::unique_ptr<Object3d>objMedama_;
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
	std::unique_ptr<Enemy> enemy_;
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
	std::unique_ptr<LevelData> levelData_;

	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;
	
#pragma endregion

};

