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

#include "GameCamera.h"

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

	void Draw2DFront();

private:// メンバ変数

	// 入力
	std::unique_ptr<Input> input_ = nullptr;

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
	Vector3 circleShadowDir = { 0,-1,0 };
	Vector3 circleShadowAtten = { 1.5f,0.6f,0.0f };
	Vector2 circleShadowFactorAngle = { 0.0f,0.5f };
	bool isActiveCircleShadow = true;

	Vector3 fighterPos = { 1,0.0f,0 };

	Vector3 AmColor={1,0,0};
	Vector3 DiColor={ 0,1,0 };
	Vector3 SpColor={ 0,0,1 };

	// テクスチャハンドル
	std::unique_ptr<Texture> textureHandle;
	std::unique_ptr<Texture> textureHandle2;

	// スプライト
	std::unique_ptr<Sprite> spriteBack_ = nullptr;
	std::unique_ptr<Sprite> sprite2 = nullptr;

	// モデル
	//Model* model = nullptr;
	std::unique_ptr<Model> model_2 = nullptr;
	
	std::unique_ptr<Model> modelMedama_ = nullptr;
	//Model* groundModel = nullptr;

	//Model* modelPlane = nullptr;
	//Model* modelBox = nullptr;
	//Model* modelPyramid = nullptr;

	// オブジェクト
	//Object3d* object3d = nullptr;
	//std::unique_ptr<Object3d> objMedama = nullptr;

	//Object3d* point1 = nullptr;
	//Object3d* point2 = nullptr;
	//Object3d* point3 = nullptr;

	//Object3d* rayobj = nullptr;

	//Player* objFighter = nullptr;
	//TouchableObject* groundObj = nullptr;
	//TouchableObject* groundObj2 = nullptr;

	//std::vector<Object3d*>objects;
	
	// ビュープロジェクション
	std::unique_ptr<GameCamera>gameCamera;



	Vector2 spritePos;
	char buf[256] = "";
	float f = 0.0f;

	std::unique_ptr<Fog> fog = nullptr;
	bool isFogActive = true;

	Sound sound;
	bool isActiveSound = false;
	bool isStopSound = false;

#pragma region 当たり判定
	CollisionManager* collisionManager = nullptr;

#pragma endregion

#pragma region ローダーの確認
	LevelData* levelData = nullptr;

	std::unique_ptr<Model> modelSkydome = nullptr;
	std::unique_ptr<Model> modelGround = nullptr;
	//Model* modelFighter = nullptr;
	//Model* modelSphere = nullptr;

	std::unique_ptr<Object3d> objSkydome = nullptr;
	std::unique_ptr<Object3d> objGround = nullptr;
	//Object3d* objFighter = nullptr;
	//Object3d* objSphere = nullptr;

	//std::map<std::string, Model*> models;
	//std::vector<Object3d*> objects;
#pragma endregion

#pragma region FBXモデルの確認
	std::unique_ptr<FbxModel> fbxmodel_;
	WorldTransform fbxTrans_;
	std::unique_ptr<FbxAnimetion> modelAnim_;
	DirectXCommon* dxCommon_ = nullptr;

	float frem = 0;
	uint32_t BoneNum = 0;
#pragma endregion


#pragma region プロトタイプ用変数
	std::unique_ptr<Player> player_;
	std::unique_ptr<Enemy> enemy_;

#pragma endregion

};

