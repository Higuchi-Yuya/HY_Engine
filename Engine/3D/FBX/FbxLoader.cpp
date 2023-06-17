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

	// FBXモデルで使うデバイスもここでセット
	FbxModel::SetDevice(device);
}

void FbxLoader::Finalize()
{
	delete sFbxLoader_;
}

FbxModel* FbxLoader::LoadModelFromFile(const string& modelName)
{
	// マトリックス型の関数を利用する
	Matrix4 mathMat;

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

	// モデル生成
	FbxModel* model = new FbxModel();
	model->name_ = modelName;

	// FBXノードの数を取得
	UINT32 nodeCount = 0;
	GetNodeNum(mScene_->mRootNode, nodeCount);

	// あらかじめ必要数分のメモリを確保することで、アドレスがずれるのを予防
	model->nodes_.reserve(nodeCount);

	model->globalInverseTransform_ = mathMat.AssimpMatToMat4(mScene_->mRootNode->mTransformation);

	// ルートノードから順に解析してモデルに流し込む
	ParseNodeRecursive(model, mScene_->mRootNode);

	// FBXシーン解放
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
	// マトリックス型の関数を利用する
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

		//ボーン自体のノードの名前を取得
		const char* boneName = meshBone->mName.C_Str();

		//新しくボーンを追加し、追加したボーンの参照を得る
		Mesh::Bone bone;

		bone.name = boneName;

		//FBXから初期姿勢行列を取得する

		//初期姿勢行列の逆行列を得る
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

	//各頂点について処理
	for (size_t j = 0; j < vertices.size(); j++)
	{
		//頂点のウェイトから最も大きい4つを選択
		auto& weightList = weightLists[j];

		size_t weightArrayIndex = 0;
		//降順ソート済みのウェイトリストから

		for (auto& weightSet : weightList)
		{
			//頂点データに書き込み
			vertices[j].boneIndex[weightArrayIndex] = weightSet.index;
			vertices[j].boneWeight[weightArrayIndex] = weightSet.weight;

			//4つに達したら修了
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
	//// ノード名を取得
	//string name = fbxNode->GetName();
	// モデルにノードを追加
	model->nodes_.emplace_back();
	Node& node = model->nodes_.back();
	// ノード名を取得
	node.name = fbxNode->mName.C_Str();

	node.transform =
	{
		fbxNode->mTransformation.a1, fbxNode->mTransformation.a2, fbxNode->mTransformation.a3, fbxNode->mTransformation.a4,
		fbxNode->mTransformation.b1, fbxNode->mTransformation.b2, fbxNode->mTransformation.b3, fbxNode->mTransformation.b4,
		fbxNode->mTransformation.c1, fbxNode->mTransformation.c2, fbxNode->mTransformation.c3, fbxNode->mTransformation.c4,
		fbxNode->mTransformation.d1, fbxNode->mTransformation.d2, fbxNode->mTransformation.d3, fbxNode->mTransformation.d4,
	};

	//グローバル変形行列の計算
	node.globalTransform = node.transform;

	if (parent) {
		node.parent = parent;
		// 親の変形を乗算
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

	// 子ノードに対して再帰呼び出し
	for (int i = 0; i < fbxNode->mNumChildren; i++) {
		ParseNodeRecursive(model, fbxNode->mChildren[i], &node);
	}
}

void FbxLoader::ParseMesh(FbxModel* model, aiMesh* fbxNode)
{
	// 頂点座標読み取り
	ParseMeshVertices(model, fbxNode);
	// 面を構成するデータの読み取り
	ParseMeshFaces(model, fbxNode);
	// マテリアルの読み取り
	ParseMaterial(model, fbxNode, mScene_->mMaterials[fbxNode->mMaterialIndex]);

	if (fbxNode->HasBones()) {
		//スキニング情報の読み取り
		ParseSkin(model, fbxNode);
	}
}

void FbxLoader::ParseMeshVertices(FbxModel* model, aiMesh* fbxMesh)
{
	auto& vertices = model->meshes_.back()->vertices_;

	// 頂点座標データの数
	const int controlPointsCount = fbxMesh->mNumVertices;

	// 必要数だけ頂点データ配列を確保
	Mesh::VertexPosNormalUv vert{};
	model->meshes_.back()->vertices_.resize(controlPointsCount, vert);

	// FBXメッシュの全頂点座標をモデル内の配列にコピーする。
	for (int i = 0; i < controlPointsCount; i++) {
		Mesh::VertexPosNormalUv& vertex = vertices[i];
		// 座標のコピー
		aiVector3D position = fbxMesh->mVertices[i];

		vertex.pos.x = position.x;
		vertex.pos.y = position.y;
		vertex.pos.z = position.z;

		// 法線のコピー
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

	// 面の数
	const int polygonCount = fbxMesh->mNumVertices;

	aiVector3D zero3D(0.0f, 0.0f, 0.0f);

	// 面ごとの情報読み取り
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
	// 区切り文字 '\\' が出てくる一番最後の部分を検索
	pos1 = path.rfind('\\');
	if (pos1 != string::npos) {
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}
	// 区切り文字 '/' が出てくる一番最後の部分を検索
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
