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
	// �������烁���o�ϐ��ɑ��
	device_ = device;
}

void FbxLoader::Finalize()
{
	delete sFbxLoader_;
}

FbxModel* FbxLoader::LoadModelFromFile(const string& modelName)
{
	// ���f���Ɠ������O�̃t�H���_����ǂݍ���
	const string directoryPath = sBaseDirectory + modelName + "/";
	// �g���q.fbx��t��
	const string fileName = modelName + ".fbx";
	// �A�����ăt���p�X�𓾂�
	const string fullpath = directoryPath + fileName;

	// FBX�t�@�C����ǂݍ��݁A���̏����V�[���ɃC���|�[�g����
	mScene_ = aiImportFile(fullpath.c_str(), flag_);

	// �V�[���������ł��Ă��邩�ǂ����m�F
	if (!mScene_) {
		assert(0);
	}

	return nullptr;
}
