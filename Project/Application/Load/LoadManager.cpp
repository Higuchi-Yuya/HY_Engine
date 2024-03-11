#include "LoadManager.h"

void LoadManager::LoadAll()
{
	// モデルの読み込み
	LoadModel();

	// レベルデータの読み込み
	LoadLevelDate();
}

void LoadManager::LoadModel()
{
#pragma region モデルの読み込み

	// 敵のモデル読み込み
	modelMedama_.reset(Model::LoadFromOBJ("Medama", true));

	// 背景モデル関連
	modelSkydome_.reset(Model::LoadFromOBJ("skydome"));
	modelGround_.reset(Model::LoadFromOBJ("ground"));

	// 木
	modelTreeBald_.reset(Model::LoadFromOBJ("tree_bald"));
	modelTreeNormal_.reset(Model::LoadFromOBJ("tree_Normal"));

	// フェンス
	modelFence_.reset(Model::LoadFromOBJ("fence"));
	modelFencePost_.reset(Model::LoadFromOBJ("fencePost"));
	modelLatticeDoor_.reset(Model::LoadFromOBJ("latticeDoor"));

	// お墓
	modelGraveCross.reset(Model::LoadFromOBJ("grave_cross"));
	modelGraveSquare.reset(Model::LoadFromOBJ("grave_square"));

	// ランタン
	modelWallRantern_.reset(Model::LoadFromOBJ("WallRantern"));
	modelPostRantern_.reset(Model::LoadFromOBJ("postRantern"));

	// 壁
	modelWall02_.reset(Model::LoadFromOBJ("wall02"));
	modelWall03_.reset(Model::LoadFromOBJ("wall03"));

	// 椅子
	modelChair_.reset(Model::LoadFromOBJ("chair"));

	// 十字架
	modelCross_.reset(Model::LoadFromOBJ("cross"));
	modelStoneCross_.reset(Model::LoadFromOBJ("stoneCross"));

#pragma endregion
}

void LoadManager::LoadLevelDate()
{
#pragma region ローダー用の読み込み
	// レベルデータの読み込み
	levelData_.reset(LevelLoader::LoadFile("testS"));

	// モデルデータをモデルのリストに登録
	models.insert(std::make_pair("skydome", modelSkydome_.get()));
	models.insert(std::make_pair("ground", modelGround_.get()));
	models.insert(std::make_pair("Medama", modelMedama_.get()));

	models.insert(std::make_pair("tree_bald", modelTreeBald_.get()));
	models.insert(std::make_pair("tree_Normal", modelTreeNormal_.get()));
	models.insert(std::make_pair("fence", modelFence_.get()));
	models.insert(std::make_pair("fencePost", modelFencePost_.get()));
	models.insert(std::make_pair("latticeDoor", modelLatticeDoor_.get()));
	models.insert(std::make_pair("grave_cross", modelGraveCross.get()));
	models.insert(std::make_pair("grave_square", modelGraveSquare.get()));
	models.insert(std::make_pair("WallRantern", modelWallRantern_.get()));

	models.insert(std::make_pair("postRantern", modelPostRantern_.get()));
	models.insert(std::make_pair("wall02", modelWall02_.get()));
	models.insert(std::make_pair("wall03", modelWall03_.get()));
	models.insert(std::make_pair("cross", modelCross_.get()));
	models.insert(std::make_pair("stoneCross", modelStoneCross_.get()));
	models.insert(std::make_pair("chair", modelChair_.get()));

	// レベルデータからオブジェクトを生成、配置
	//	また、プレイヤーの初期位置やエネミーの初期
	for (auto& objectData : levelData_->objects) {
		// ファイル名から登録済みモデルを検索
		Model* model = nullptr;
		decltype(models)::iterator it = models.find(objectData.fileName);
		if (it != models.end()) {
			model = it->second;
		}

		if (objectData.tagName == "enemy" ||
			objectData.tagName == "ItemKey" ||
			objectData.tagName == "ItemPaper") {

		}
		else if (objectData.tagName == "colBox") {

			Box box;
			box.center = objectData.translation;
			box.radius = objectData.scaling;

			gameCollider_->AddObjBox(box);
		}
		else if (objectData.tagName == "colSphere") {

			Sphere sphere;
			sphere.center = objectData.translation;
			sphere.radius = objectData.scaling.x;

			gameCollider_->AddObjSphere(sphere);
		}
		// タグ名がドアなら
#pragma region お墓のドア
		else if (objectData.tagName == "Door")
		{
			// モデルを指定して3Dオブジェクトを生成
			DeferredObject3d* newObject = DeferredObject3d::Create();
			newObject->SetModel(model);

			// 座標
			Vector3 pos;
			pos = objectData.translation;
			newObject->worldTransform_.translation = pos;

			// 回転角
			Vector3 rot;
			rot = objectData.rotation;
			newObject->worldTransform_.rotation = rot;
			newObject->worldTransform_.rotation.y += MathUtil::DegreeToRadian(180);

			// スケール
			Vector3 scale;
			scale = objectData.scaling;
			newObject->worldTransform_.scale = scale;

			// 配列に登録
			latticeDoors_.push_back(newObject);
		}
#pragma endregion

		// タグ名がランタンなら
		else if (objectData.tagName == "Rantern")
		{
			// モデルを指定して3Dオブジェクトを生成
			DeferredObject3d* newObject = DeferredObject3d::Create();
			Object3d* newObj2 = Object3d::Create();
			newObject->SetModel(model);
			newObj2->SetModel(model);

			// 座標
			Vector3 pos;
			pos = objectData.translation;
			newObject->worldTransform_.translation = pos;
			newObj2->worldTransform_.translation = pos;

			// 回転角
			Vector3 rot;
			rot = objectData.rotation;
			newObject->worldTransform_.rotation = rot;
			newObj2->worldTransform_.rotation = rot;
			//newObject->worldTransform_.rotation.y += MathUtil::DegreeToRadian(180);

			// スケール
			Vector3 scale;
			scale = objectData.scaling;
			newObject->worldTransform_.scale = scale;
			newObj2->worldTransform_.scale = scale;

			// 配列に登録
			ranterns_.push_back(newObject);
			highLumiRanterns_.push_back(newObj2);
		}

		// タグ名がポイントライトなら
		else if (objectData.tagName == "PointLight")
		{
			// ポイントライトの登録
			WorldTransform* newWorldTrans = new WorldTransform();
			newWorldTrans->Initialize();

			newWorldTrans->translation = objectData.translation;

			// 配列に登録
			pointLightsInfo_.push_back(newWorldTrans);

		}

		// それ以外のタグ名またはなしの場合
		// 普通のオブジェクトと判断し生成
		else
		{
			// モデルを指定して3Dオブジェクトを生成
			DeferredObject3d* newObject = DeferredObject3d::Create();
			newObject->SetModel(model);

			// 座標
			Vector3 pos;
			pos = objectData.translation;
			newObject->worldTransform_.translation = pos;

			// 回転角
			Vector3 rot;
			rot = objectData.rotation;
			newObject->worldTransform_.rotation = rot;

			// スケール
			Vector3 scale;
			scale = objectData.scaling;
			newObject->worldTransform_.scale = scale;

			// 配列に登録
			objects.push_back(newObject);

			// タグ名が墓もしくは木だったら、当たり判定をつける
			if (objectData.tagName == "tree" ||
				objectData.tagName == "grave") {
				gameCollider_->AddObj(newObject);
			}
		}

	}
#pragma endregion
}

void LoadManager::SetCollider(GameCollider* collider)
{
	gameCollider_ = collider;
}

Model* LoadManager::GetModel(std::string modelName)
{
	auto it = models.find(modelName);

	if (it != models.end()) {
		return nullptr;
	}

	return models[modelName];
}

std::map<std::string, Model*> LoadManager::GetModels()
{
	return models;
}

std::vector<DeferredObject3d*> LoadManager::GetObjects()
{
	return objects;
}

std::vector<DeferredObject3d*> LoadManager::GetLatticeDoors()
{
	return latticeDoors_;
}

std::vector<DeferredObject3d*> LoadManager::GetRanterns()
{
	return ranterns_;
}

std::vector<Object3d*> LoadManager::GetHighRanterns()
{
	return highLumiRanterns_;
}

std::vector<WorldTransform*> LoadManager::GetPointLightInfo()
{
	return pointLightsInfo_;
}

LoadManager::LoadManager() {

}

void LoadManager::Finalize()
{
	models.clear();

	for (auto o : objects) {
		delete o;
	}
	objects.clear();

	for (auto o : latticeDoors_) {
		delete o;
	}
	latticeDoors_.clear();

	for (auto o : ranterns_) {
		delete o;
	}
	ranterns_.clear();

	for (auto o : highLumiRanterns_) {
		delete o;
	}
	highLumiRanterns_.clear();

	for (auto o : pointLightsInfo_) {
		delete o;
	}
	pointLightsInfo_.clear();
}
