#include "FbxAnimetion.h"
#include <cassert>

FbxAnimetion::FbxAnimetion()
{
}

FbxAnimetion::~FbxAnimetion()
{
}

void FbxAnimetion::Load(const std::string& failPath)
{
	// ���f���Ɠ������O�̃t�H���_����ǂݍ���
	const std::string directoryPath = baseDirectory + failPath + "/";
	// �g���q.fbx��t��
	const std::string fileName = failPath + ".fbx";
	// �A�����ăt���p�X�𓾂�
	const std::string fullpath = directoryPath + fileName;

	UINT flag = 0;
	flag |= aiProcess_Triangulate;//�O�p�ʉ�
	flag |= aiProcess_CalcTangentSpace;//�ڐ��x�N�g������
	flag |= aiProcess_GenSmoothNormals;//�X���[�W���O�x�N�g������
	flag |= aiProcess_GenUVCoords;//��}�b�s���O��K�؂�UV���W�ɕϊ�
	flag |= aiProcess_RemoveRedundantMaterials;//�璷�ȃ}�e���A�����폜
	flag |= aiProcess_OptimizeMeshes;//���b�V�������œK��
	flag |= aiProcess_MakeLeftHanded;//�m�[�h��������W�n��

	mScene = aiImportFile(fullpath.c_str(), flag);

	// �t�@�C�������w�肵��FBX�t�@�C����ǂݍ���
	if (!mScene) {
		assert(0);
	}

	for (uint32_t i = 0; i < mScene->mNumAnimations; i++) {
		modelAnimation[i] = mScene->mAnimations[i];
	}
}

aiAnimation* FbxAnimetion::GetAnimation(uint32_t AnimationNumber)
{
	return modelAnimation[AnimationNumber];
}
