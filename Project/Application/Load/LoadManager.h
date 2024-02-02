#pragma once
#include "Object3d.h"
#include "LevelLoder.h"
#include "Singleton.h"
#include <GameCollider.h>

class LoadManager
{
public:
	/// <summary>
	/// 全ての読み込み処理
	/// </summary>
	void LoadAll();

private:
	/// <summary>
	/// モデルの読み込み
	/// </summary>
	void LoadModel();


	/// <summary>
	/// レベルデータ
	/// </summary>
	void LoadLevelDate();

public:// セッター
	// ゲームコライダーのセット
	void SetCollider(GameCollider* collider);

public://ゲッター
	// ゲッター

	/// <summary>
	/// モデルのゲッター
	/// </summary>
	/// <param name="modelName">モデルの名前</param>
	/// <returns></returns>
	Model* GetModel(std::string modelName);

	/// <summary>
	/// モデルデータ全てを取得
	/// </summary>
	/// <returns></returns>
	std::map<std::string, Model*>GetModels();

	/// <summary>
	/// 配置オブジェクトの取得
	/// </summary>
	/// <returns></returns>
	std::vector<Object3d*>GetObjects();

	/// <summary>
	/// 門のオブジェクトの取得
	/// </summary>
	std::vector<Object3d*>GetLatticeDoors();

	/// <summary>
	/// ランタンのオブジェクトの取得
	/// </summary>
	/// <returns></returns>
	std::vector<Object3d*>GetRanterns();

	/// <summary>
	/// 高輝度ランタンのオブジェクトの取得
	/// </summary>
	/// <returns></returns>
	std::vector<Object3d*>GetHighRanterns();

	/// <summary>
	/// ポイントライトの情報
	/// </summary>
	/// <returns></returns>
	std::vector<WorldTransform*>GetPointLightInfo();

private:

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

	// ゲームコライダー
	GameCollider* gameCollider_;


	
public:
	LoadManager();
	void Finalize();

};