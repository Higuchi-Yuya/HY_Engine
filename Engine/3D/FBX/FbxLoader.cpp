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
	// �������烁���o�ϐ��ɑ��
	this->device = device;
}

void FbxLoader::Finalize()
{
	delete fbxLoader_;
}

//FbxModel* FbxLoader::LoadModelFromFile(const string& modelName)
//{
//	// ���f���Ɠ������O�̃t�H���_����ǂݍ���
//	const string directoryPath = baseDirectory + modelName + "/";
//	// �g���q.fbx��t��
//	const string fileName = modelName + ".fbx";
//	// �A�����ăt���p�X�𓾂�
//	const string fullpath = directoryPath + fileName;
//
//	// FBX�t�@�C����ǂݍ��݁A���̏����V�[���ɃC���|�[�g����
//	mScene = aiImportFile(fullpath.c_str(), flag);
//
//	// �V�[���������ł��Ă��邩�ǂ����m�F
//	if (!mScene) {
//		assert(0);
//	}
//
//	return nullptr;
//}
