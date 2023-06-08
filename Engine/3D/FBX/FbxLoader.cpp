#include "FbxLoader.h"
#include <cassert>

FbxLoader* FbxLoader::sFbxLoader_;
const std::string FbxLoader::sBaseDirectory = "Resources/3D_Resources/FBX_File/";

FbxLoader* FbxLoader::GetInstance()
{
	if (sFbxLoader_ == nullptr)
	{
		sFbxLoader_ = new FbxLoader();
	}

	return sFbxLoader_;
}

void FbxLoader::Initialize(ID3D12Device* device)
{
	// 引数からメンバ変数に代入
	device_ = device;
}

void FbxLoader::Finalize()
{
	delete sFbxLoader_;
}

FbxModel* FbxLoader::LoadModelFromFile(const string& modelName)
{
	// モデルと同じ名前のフォルダから読み込む
	const string directoryPath = sBaseDirectory + modelName + "/";
	// 拡張子.fbxを付加
	const string fileName = modelName + ".fbx";
	// 連結してフルパスを得る
	const string fullpath = directoryPath + fileName;

	// FBXファイルを読み込み、その情報をシーンにインポートする
	mScene_ = aiImportFile(fullpath.c_str(), flag_);

	// シーンが生成できているかどうか確認
	if (!mScene_) {
		assert(0);
	}

	return nullptr;
}
