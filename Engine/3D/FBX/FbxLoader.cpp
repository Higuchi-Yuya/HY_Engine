#include "FbxLoader.h"
#include <cassert>

FbxLoader* FbxLoader::fbxLoader_;
const std::string FbxLoader::baseDirectory = "Resources/3D_Resources/FBX_File/";

FbxLoader* FbxLoader::GetInstance()
{
	if (fbxLoader_ == nullptr)
	{
		fbxLoader_ = new FbxLoader();
	}

	return fbxLoader_;
}

void FbxLoader::Initialize(ID3D12Device* device)
{
	// 引数からメンバ変数に代入
	this->device = device;
}

void FbxLoader::Finalize()
{
	delete fbxLoader_;
}

//FbxModel* FbxLoader::LoadModelFromFile(const string& modelName)
//{
//	// モデルと同じ名前のフォルダから読み込む
//	const string directoryPath = baseDirectory + modelName + "/";
//	// 拡張子.fbxを付加
//	const string fileName = modelName + ".fbx";
//	// 連結してフルパスを得る
//	const string fullpath = directoryPath + fileName;
//
//	// FBXファイルを読み込み、その情報をシーンにインポートする
//	mScene = aiImportFile(fullpath.c_str(), flag);
//
//	// シーンが生成できているかどうか確認
//	if (!mScene) {
//		assert(0);
//	}
//
//	return nullptr;
//}
