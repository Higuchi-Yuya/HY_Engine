#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>
#include "WorldTransform.h"
#include <Vector2.h>

// レベルデータ
struct LevelData {

	struct ObjectData {
		// ファイル名
		std::string fileName;
		// タグ名
		std::string tagName;
		// アイテムのテクスチャ名
		std::string itemTexName;
		// 平行移動
		Vector3 translation;
		// 回転角
		Vector3 rotation;
		// スケーリング
		Vector3 scaling;
	};

	// オブジェクト配列
	std::vector<ObjectData> objects;
};

// レベルデータのローダー
class LevelLoader {

public:// 定数
	// デフォルトの読み込みディレクトリ
	static const std::string kDefaultBaseDirectory;
	// ファイル拡張子
	static const std::string kExtension;

public:// メンバ関数

	/// <summary>
	/// レベルデータファイルの読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	static LevelData* LoadFile(const std::string& fileName);


	/// <summary>
	/// int のデータの読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <param name="variableName">変数名</param>
	/// <returns></returns>
	static int LoadIntData(const std::string& fileName,const std::string& variableName);

	/// <summary>
	/// float のデータの読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <param name="variableName">変数名</param>
	/// <returns></returns>
	static float LoadFloatData(const std::string& fileName, const std::string& variableName);

	/// <summary>
	/// Vector2 のデータの読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <param name="variableName">変数名</param>
	/// <returns></returns>
	static Vector2 LoadVec2Data(const std::string& fileName, const std::string& variableName);

	/// <summary>
	/// Vector3 のデータの読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <param name="variableName">変数名</param>
	/// <returns></returns>
	static Vector3 LoadVec3Data(const std::string& fileName, const std::string& variableName);

	/// <summary>
	/// Vector4 のデータの読み込み
	/// </summary>
	/// <param name="fileName">ファイル名</param>
	/// <param name="variableName">変数名</param>
	/// <returns></returns>
	static Vector4 LoadVec4Data(const std::string& fileName, const std::string& variableName);
};

