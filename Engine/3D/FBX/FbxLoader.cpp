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

	// FBX���f���Ŏg���f�o�C�X�������ŃZ�b�g
	FbxModel::SetDevice(device);
}

void FbxLoader::Finalize()
{
	delete sFbxLoader_;
}

FbxModel* FbxLoader::LoadModelFromFile(const string& modelName)
{
	// �}�g���b�N�X�^�̊֐��𗘗p����
	Matrix4 mathMat;

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

	// ���f������
	FbxModel* model = new FbxModel();
	model->name_ = modelName;

	// FBX�m�[�h�̐����擾
	UINT32 nodeCount = 0;
	GetNodeNum(mScene_->mRootNode, nodeCount);

	// ���炩���ߕK�v�����̃��������m�ۂ��邱�ƂŁA�A�h���X�������̂�\�h
	model->nodes_.reserve(nodeCount);

	model->globalInverseTransform_ = mathMat.AssimpMatToMat4(mScene_->mRootNode->mTransformation);

	// ���[�g�m�[�h���珇�ɉ�͂��ă��f���ɗ�������
	ParseNodeRecursive(model, mScene_->mRootNode);

	// FBX�V�[�����
	aiReleaseImport(mScene_);

	for (size_t i = 0; i < model->nodes_.size(); i++)
	{
		if (model->nodes_[i].parent)
		{
			auto itr = std::find_if(model->nodes_.begin(), model->nodes_.end(), [&](Node& node)
				{
					return node.name == model->nodes_[i].parent->name;
				});

			itr->childrens.push_back(&model->nodes_[i]);
		}
	}

	model->SetTextureHandle(textureHandle_);

	return nullptr;
}

void FbxLoader::ParseSkin(FbxModel* model, aiMesh* fbxMesh)
{
	// �}�g���b�N�X�^�̊֐��𗘗p����
	Matrix4 mathMat;

	auto& vertices = model->meshes_.back()->vertices_;

	struct WeightSet {
		UINT index;
		float weight;
	};

	std::vector<std::list<WeightSet>> weightLists(vertices.size());

	if (fbxMesh->mNumBones == 0) {
		return;
	}

	for (int i = 0; i < fbxMesh->mNumBones; i++) {

		auto& meshBone = fbxMesh->mBones[i];

		//�{�[�����̂̃m�[�h�̖��O���擾
		const char* boneName = meshBone->mName.C_Str();

		//�V�����{�[����ǉ����A�ǉ������{�[���̎Q�Ƃ𓾂�
		Mesh::Bone bone;

		bone.name = boneName;

		//FBX���珉���p���s����擾����

		//�����p���s��̋t�s��𓾂�
		bone.offsetMatirx = mathMat.AssimpMatToMat4(meshBone->mOffsetMatrix.Transpose());



		bone.index = i;

		model->meshes_.back()->vecBones.push_back(bone);
		//model->meshes_.back()->bones[bone.name] = &model->meshes_.back()->vecBones.back();

		for (int j = 0; j < meshBone->mNumWeights; j++) {
			int vertIndex = meshBone->mWeights[j].mVertexId;

			float weight = (float)meshBone->mWeights[j].mWeight;

			weightLists[vertIndex].emplace_back(WeightSet{ (UINT)i,weight });
		}
	}

	//�e���_�ɂ��ď���
	for (size_t j = 0; j < vertices.size(); j++)
	{
		//���_�̃E�F�C�g����ł��傫��4��I��
		auto& weightList = weightLists[j];

		size_t weightArrayIndex = 0;
		//�~���\�[�g�ς݂̃E�F�C�g���X�g����

		for (auto& weightSet : weightList)
		{
			//���_�f�[�^�ɏ�������
			vertices[j].boneIndex[weightArrayIndex] = weightSet.index;
			vertices[j].boneWeight[weightArrayIndex] = weightSet.weight;

			//4�ɒB������C��
			if (++weightArrayIndex >= Mesh::sMAX_BONE_INDICES)
			{
				break;
			}

		}
	}
}

void FbxLoader::GetNodeNum(const aiNode* node, UINT32& num)
{
	if (node->mChildren)
	{
		for (UINT32 i = 0; i < node->mNumChildren; i++)
		{
			GetNodeNum(node->mChildren[i], num);
		}
	}

	num++;
}

void FbxLoader::ParseNodeRecursive(FbxModel* model, aiNode* fbxNode, Node* parent)
{
	//// �m�[�h�����擾
	//string name = fbxNode->GetName();
	// ���f���Ƀm�[�h��ǉ�
	model->nodes_.emplace_back();
	Node& node = model->nodes_.back();
	// �m�[�h�����擾
	node.name = fbxNode->mName.C_Str();

	node.transform =
	{
		fbxNode->mTransformation.a1, fbxNode->mTransformation.a2, fbxNode->mTransformation.a3, fbxNode->mTransformation.a4,
		fbxNode->mTransformation.b1, fbxNode->mTransformation.b2, fbxNode->mTransformation.b3, fbxNode->mTransformation.b4,
		fbxNode->mTransformation.c1, fbxNode->mTransformation.c2, fbxNode->mTransformation.c3, fbxNode->mTransformation.c4,
		fbxNode->mTransformation.d1, fbxNode->mTransformation.d2, fbxNode->mTransformation.d3, fbxNode->mTransformation.d4,
	};

	//�O���[�o���ό`�s��̌v�Z
	node.globalTransform = node.transform;

	if (parent) {
		node.parent = parent;
		// �e�̕ό`����Z
		node.globalTransform *= parent->globalTransform;
	}

	for (UINT32 i = 0; i < fbxNode->mNumMeshes; i++) {
		aiMesh* aimesh = mScene_->mMeshes[fbxNode->mMeshes[i]];
		if (aimesh) {
			model->meshes_.emplace_back();
			model->meshes_.back() = new Mesh();

			model->meshes_.back()->name_ = aimesh->mName.C_Str();

			model->meshes_.back()->node = &node;

			ParseMesh(model, aimesh);
		}
	}

	// �q�m�[�h�ɑ΂��čċA�Ăяo��
	for (int i = 0; i < fbxNode->mNumChildren; i++) {
		ParseNodeRecursive(model, fbxNode->mChildren[i], &node);
	}
}

void FbxLoader::ParseMesh(FbxModel* model, aiMesh* fbxNode)
{
	// ���_���W�ǂݎ��
	ParseMeshVertices(model, fbxNode);
	// �ʂ��\������f�[�^�̓ǂݎ��
	ParseMeshFaces(model, fbxNode);
	// �}�e���A���̓ǂݎ��
	ParseMaterial(model, fbxNode, mScene_->mMaterials[fbxNode->mMaterialIndex]);

	if (fbxNode->HasBones()) {
		//�X�L�j���O���̓ǂݎ��
		ParseSkin(model, fbxNode);
	}
}

void FbxLoader::ParseMeshVertices(FbxModel* model, aiMesh* fbxMesh)
{
	auto& vertices = model->meshes_.back()->vertices_;

	// ���_���W�f�[�^�̐�
	const int controlPointsCount = fbxMesh->mNumVertices;

	// �K�v���������_�f�[�^�z����m��
	Mesh::VertexPosNormalUv vert{};
	model->meshes_.back()->vertices_.resize(controlPointsCount, vert);

	// FBX���b�V���̑S���_���W�����f�����̔z��ɃR�s�[����B
	for (int i = 0; i < controlPointsCount; i++) {
		Mesh::VertexPosNormalUv& vertex = vertices[i];
		// ���W�̃R�s�[
		aiVector3D position = fbxMesh->mVertices[i];

		vertex.pos.x = position.x;
		vertex.pos.y = position.y;
		vertex.pos.z = position.z;

		// �@���̃R�s�[
		aiVector3D normal = fbxMesh->mNormals[i];

		vertex.normal.x = normal.x;
		vertex.normal.y = normal.y;
		vertex.normal.z = normal.z;

	}
}

void FbxLoader::ParseMeshFaces(FbxModel* model, aiMesh* fbxMesh)
{
	auto& vertices = model->meshes_.back()->vertices_;
	auto& indices = model->meshes_.back()->indices_;

	// �ʂ̐�
	const int polygonCount = fbxMesh->mNumVertices;

	aiVector3D zero3D(0.0f, 0.0f, 0.0f);

	// �ʂ��Ƃ̏��ǂݎ��
	for (int i = 0; i < polygonCount; i++) {
		aiVector3D* uv = (fbxMesh->HasTextureCoords(0)) ? &(fbxMesh->mTextureCoords[0][i]) : &zero3D;

		vertices[i].uv = Vector2(uv->x, -uv->y);
	}

	indices.resize(fbxMesh->mNumFaces * 3);

	for (UINT i = 0; i < fbxMesh->mNumFaces; i++) {
		const aiFace& face = fbxMesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++) {
			indices[i * 3 + j] = face.mIndices[j];
		}

	}
}

void FbxLoader::ParseMaterial(FbxModel* model, aiMesh* fbxMesh, aiMaterial* aimaterial)
{
	auto& material = model->meshes_.back()->material_;

	material = Material::Create();

	aiColor3D ambient(0.3f, 0.3f, 0.3f);
	aimaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
	material->ambient = Vector3(ambient.r, ambient.g, ambient.b);

	aiColor3D diffuse(0.0f, 0.0f, 0.0f);
	aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
	material->diffuse = Vector3(diffuse.r, diffuse.g, diffuse.b);

	aiColor3D specular(0.0f, 0.0f, 0.0f);
	aimaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
	material->specular = Vector3(specular.r, specular.g, specular.b);

	aiString str;
	aimaterial->Get(AI_MATKEY_NAME, str);
	material->name = str.C_Str();

	Texture deffuseMap = LoadMatrixerialTextures(aimaterial, aiTextureType_DIFFUSE, "Diffuse", mScene_, model->name_);

	material->textureIndex = deffuseMap;
}

std::string FbxLoader::ExtractFileName(const std::string& path)
{
	size_t pos1;
	// ��؂蕶�� '\\' ���o�Ă����ԍŌ�̕���������
	pos1 = path.rfind('\\');
	if (pos1 != string::npos) {
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}
	// ��؂蕶�� '/' ���o�Ă����ԍŌ�̕���������
	pos1 = path.rfind('/');
	if (pos1 != string::npos) {
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	return path;
}

Texture FbxLoader::LoadMatrixerialTextures(aiMaterial* cmatrix, aiTextureType type, std::string typeName, const aiScene* scene_, const std::string& modelName)
{
	Texture textures;

	for (size_t i = 0; i < cmatrix->GetTextureCount(type); i++)
	{
		aiString str;
		cmatrix->GetTexture(type, static_cast<UINT>(i), &str);
		Texture texture;
		{
			std::string filename = ExtractFileName(std::string(str.C_Str()));
			filename = modelName + '\\' + filename;
			texture = TextureManager::LoadTexture(filename);
			textureHandle_ = texture;
		}

		textures = texture;
	}
	return textures;
}
