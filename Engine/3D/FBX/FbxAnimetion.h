#pragma once
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>
#include<assimp/cimport.h>

#include <d3d12.h>
#include <d3dx12.h>

class FbxAnimetion
{
public:

	FbxAnimetion();
	~FbxAnimetion();

	void Load(const std::string& failPath);

	aiAnimation* GetAnimation(uint32_t AnimationNumber);

public: // �萔
	// ���f���i�[���[�g�p�X
	static const std::string baseDirectory;
	// �e�N�X�`�����Ȃ��ꍇ�̕W���e�N�X�`���t�@�C����
	static const std::string defaultTextureFileName;

private:

	static const int32_t MaxAnimation = 20;

	aiAnimation* modelAnimation[MaxAnimation];
	const aiScene* mScene;
};

