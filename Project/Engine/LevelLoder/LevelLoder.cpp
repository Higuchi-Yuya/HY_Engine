#include "LevelLoder.h"

#include "json.hpp"
#include <fstream>
#include <cassert>
#include "MathUtil.h"


const std::string LevelLoader::kDefaultBaseDirectory = "Resources/Levels/";
const std::string LevelLoader::kExtension = ".json";

LevelData* LevelLoader::LoadFile(const std::string& fileName) {
	// 連結してフルパスを得る
	const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;

	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullpath);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	// JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	// 解凍
	file >> deserialized;

	// 正しいレベルデータファイルかチェック
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	// "name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();
	// 正しいレベルデータファイルかチェック
	assert(name.compare("scene") == 0);

	// レベルデータ格納用インスタンスを生成
	LevelData* levelData = new LevelData();

	// "objects"の全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		// 種別を取得
		std::string type = object["type"].get<std::string>();

		// MESH
		if (type.compare("MESH") == 0) {
			// 要素追加
			levelData->objects.emplace_back(LevelData::ObjectData{});
			// 今追加した要素の参照を得る
			LevelData::ObjectData& objectData = levelData->objects.back();

			if (object.contains("file_name")) {
				// ファイル名
				objectData.fileName = object["file_name"];
			}
			if (object.contains("tag_name")) {
				// タグ名
				objectData.tagName = object["tag_name"];
			}
			if (object.contains("itemTex_name")) {
				// アイテムテクスチャ名
				objectData.itemTexName = object["itemTex_name"];
			}
			// トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			// 平行移動
			objectData.translation.x = (float)transform["translation"][1];
			objectData.translation.y = (float)transform["translation"][2];
			objectData.translation.z = -(float)transform["translation"][0];
			
			// 回転角
			objectData.rotation.x = MathUtil::DegreeToRadian((float)transform["rotation"][0] - 90);
			objectData.rotation.y = -MathUtil::DegreeToRadian((float)transform["rotation"][2] - 90);
			objectData.rotation.z = MathUtil::DegreeToRadian((float)transform["rotation"][1] );

			// スケーリング
			objectData.scaling.x = (float)transform["scaling"][1];
			objectData.scaling.y = (float)transform["scaling"][2];
			objectData.scaling.z = (float)transform["scaling"][0];
	

			// TODO: コライダーのパラメータ読み込み
		}

		// TODO: オブジェクト走査を再帰関数にまとめ、再帰呼出で枝を走査する
		if (object.contains("children")) {

		}
	}

	return levelData;
}

int LevelLoader::LoadIntData(const std::string& fileName, const std::string& variableName)
{
	// TODO: return ステートメントをここに挿入します
	
	// 連結してフルパスを得る
	const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;

	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullpath);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	// JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	// 解凍
	file >> deserialized;

	//正しいファイルかチェック
	assert(deserialized.is_object());

	// データを格納
	int data;

	// intのパラメータ読み込み
	nlohmann::json& int_ = deserialized["int"];

	// 変数名に適応したもののデータを格納
	data = int_[variableName];

	// 値を返す
	return data;
}

float LevelLoader::LoadFloatData(const std::string& fileName, const std::string& variableName)
{
	// 連結してフルパスを得る
	const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;

	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullpath);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	// JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	// 解凍
	file >> deserialized;

	//正しいファイルかチェック
	assert(deserialized.is_object());

	// データを格納
	float data;

	// floatのパラメータ読み込み
	nlohmann::json& float_ = deserialized["float"];

	// 変数名に適応したもののデータを格納
	data = float_[variableName];

	// 値を返す
	return data;
}

Vector2 LevelLoader::LoadVec2Data(const std::string& fileName, const std::string& variableName)
{
	// 連結してフルパスを得る
	const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;

	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullpath);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	// JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	// 解凍
	file >> deserialized;

	//正しいファイルかチェック
	assert(deserialized.is_object());

	// データを格納
	Vector2 data;

	// vec2のパラメータ読み込み
	nlohmann::json& vec2 = deserialized["Vec2"];

	// 変数名に適応したもののデータを格納
	data.x = vec2[variableName][0];
	data.y = vec2[variableName][1];

	// 値を返す
	return data;
}

Vector3 LevelLoader::LoadVec3Data(const std::string& fileName, const std::string& variableName)
{
	// 連結してフルパスを得る
	const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;

	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullpath);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	// JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	// 解凍
	file >> deserialized;

	//正しいファイルかチェック
	assert(deserialized.is_object());

	// データを格納
	Vector3 data;

	// vec3のパラメータ読み込み
	nlohmann::json& vec3 = deserialized["Vec3"];

	// 変数名に適応したもののデータを格納
	data.x = vec3[variableName][0];
	data.y = vec3[variableName][1];
	data.z = vec3[variableName][2];

	// 値を返す
	return data;
}

Vector4 LevelLoader::LoadVec4Data(const std::string& fileName, const std::string& variableName)
{
	// 連結してフルパスを得る
	const std::string fullpath = kDefaultBaseDirectory + fileName + kExtension;

	// ファイルストリーム
	std::ifstream file;

	// ファイルを開く
	file.open(fullpath);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	// JSON文字列から解凍したデータ
	nlohmann::json deserialized;

	// 解凍
	file >> deserialized;

	//正しいファイルかチェック
	assert(deserialized.is_object());

	// データを格納
	Vector4 data;

	// vec4のパラメータ読み込み
	nlohmann::json& vec4 = deserialized["Vec4"];

	// 変数名に適応したもののデータを格納
	data.x = vec4[variableName][0];
	data.y = vec4[variableName][1];
	data.z = vec4[variableName][2];
	data.w = vec4[variableName][3];

	// 値を返す
	return data;
}

