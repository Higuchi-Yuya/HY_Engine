#include "Model.h"
#include <DirectXTex.h>
#include <windows.h>
#include <fstream>
#include <sstream>
#include <cassert>
#include "Texture.h"
#include "assimp/aabb.h"

using namespace DirectX;
using namespace std;

ID3D12Device* Model::device = nullptr;

Model::Model()
{
}

Model::~Model()
{
	
	for (auto m : materials) {
		delete m.second;
	}
	// 全メッシュを削除
	for (auto mesh : meshes) {
		delete mesh;
	}
	materials.clear();
}

Model* Model::LoadFromOBJ(const std::string& modelname, bool smoothing)
{
	Model* model = new Model;

	// オブジェクトファイルからデータを読み込む
	model->LoadFromOBJInternal(modelname, smoothing);

	// バッファ生成
	model->CreateBuffers();

	return model;
}

void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	//ファイルストリーム
	std::ifstream file;
	// マテリアルファイルを開く
	file.open(directoryPath + filename);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	Material* material = nullptr;

	// 1行ずつ読み込む
	string line;
	while (getline(file, line)) {
		// 一行分の文字列をストリームに変換
		std::istringstream line_stream(line);
		// 半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		// 先頭のタブ文字は無視する
		if (key[0] == '\t') {
			key.erase(key.begin());// 先頭の文字を削除
		}

		// 先頭文字列がnewmtlならマテリアル名
		if (key == "newmtl") {
			// 既にマテリアルがあれば
			if (material) {
				// マテリアルをコンテナに登録
				AddMaterial(material);
			}

			// 新しいマテリアルを生成
			material = Material::Create();

			// マテリアル名読み込み
			line_stream >> material->name;
		}

		// 先頭文字列がKsならアンビエント色
		if (key == "Ka") {
			line_stream >> material->ambient.x;
			line_stream >> material->ambient.y;
			line_stream >> material->ambient.z;

		}
		// 先頭文字列がKsならディフューズ色
		if (key == "Kd") {
			line_stream >> material->diffuse.x;
			line_stream >> material->diffuse.y;
			line_stream >> material->diffuse.z;

		}

		// 先頭文字列がKsならスペキュラー色
		if (key == "Ks") {
			line_stream >> material->specular.x;
			line_stream >> material->specular.y;
			line_stream >> material->specular.z;

		}
		// 先頭文字列がmap_Kdならテクスチャファイル名
		if (key == "map_Kd") {
			// テクスチャのファイル名読み込み
			line_stream >> material->textureFilename;
			// テクスチャ読み込み
			material->LoadTexture(directoryPath, material->textureFilename);
		}
	}
	// ファイルを閉じる
	file.close();

	if (material) {
		// マテリアルを登録
		AddMaterial(material);
	}
}

void Model::Draw(ID3D12GraphicsCommandList* cmdList, UINT rootParamIndexMaterial)
{
	// nullptrチェック
	assert(device);
	assert(cmdList);

	// 全メッシュを描画
	for (auto& mesh : meshes) {
		mesh->Draw(cmdList);
	}
}



void Model::LoadFromOBJInternal(const std::string& modelname, bool smoothing)
{
	// ファイルストリーム
	std::ifstream file;

	// objファイルを開く
	//const string modelname = "skydome";
	const string filename = modelname + ".obj"; // "triangle_mat.obj"
	const string directoryPath = "Resources/" + modelname + "/"; // "Resources/triangle_mat/"
	file.open(directoryPath + filename); //"Resources/triangle_mat/triangle_mat.obj"
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	name_ = modelname;

	// メッシュ生成
	meshes.emplace_back(new Mesh);
	Mesh* mesh = meshes.back();
	int indexCountTex = 0;
	int indexCountNoTex = 0;

	vector<Vector3> positions; // 頂点座標
	vector<Vector3> normals; // 法線ベクトル
	vector<Vector2> texcoords; // テクスチャUV
	// 1行ずつ読み込む
	string line;
	while (getline(file, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		// 半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		// 先頭文字列がmtllibならマテリアル
		if (key == "mtllib")
		{
			// マテリアルのファイル名読み込み
			string filename;
			line_stream >> filename;

			// マテリアル読み込み
			LoadMaterial(directoryPath, filename);
		}
		// 先頭文字列がgならグループの開始
		if (key == "g") {

			// カレントメッシュの情報が揃っているなら
			if (mesh->GetName().size() > 0 && mesh->GetVertexCount() > 0) {
				// 頂点法線の平均によるエッジの平滑化
				if (smoothing) {
					mesh->CalculateSmoothedVertexNormals();
				}
				// 次のメッシュ生成
				meshes.emplace_back(new Mesh);
				mesh = meshes.back();
				indexCountTex = 0;
			}

			// グループ名読み込み
			string groupName;
			line_stream >> groupName;

			// メッシュに名前をセット
			mesh->SetName(groupName);
		}
		// 先頭文字列がvなら頂点座標
		if (key == "v") {
			// X,Y,Z座標読み込み
			Vector3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;

			// 座標データに追加
			positions.emplace_back(position);
		}

		// 先頭文字列がvtならテクスチャ
		if (key == "vt") {
			// U,V成分読み込み
			Vector2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;

			// V方向反転
			texcoord.y = 1.0f - texcoord.y;
			// テクスチャ座標データに追加
			texcoords.emplace_back(texcoord);

		}

		// 先頭文字列がVNなら法線ベクトル
		if (key == "vn") {
			// X,Y,Z成分読み込み
			Vector3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			// 法線ベクトルデータに追加
			normals.emplace_back(normal);
		}
		// 先頭文字列がusemtlならマテリアルを割り当てる
		if (key == "usemtl") {
			if (mesh->GetMaterial() == nullptr) {
				// マテリアルの名読み込み
				string materialName;
				line_stream >> materialName;

				// マテリアル名で検索し、マテリアルを割り当てる
				auto itr = materials.find(materialName);
				if (itr != materials.end()) {
					mesh->SetMaterial(itr->second);
				}
			}
		}
		// 先頭文字列がfならポリゴン（三角形）
		if (key == "f") {
			int faceIndexCount = 0;
			// 半角スペース区切りで行の続きを読み込む
			string index_string;
			while (getline(line_stream, index_string, ' ')) {
				// 頂点インデックス１個分の文字列をストリームに変換して解析しやすくする
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				// 頂点番号
				index_stream >> indexPosition;

				Material* material = mesh->GetMaterial();
				index_stream.seekg(1, ios_base::cur);// スラッシュを飛ばす
				// マテリアル、テクスチャがある場合
				if (materials.size() > 0) {
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur);// スラッシュを飛ばす
					index_stream >> indexNormal;
					// 頂点データの追加
					Mesh::VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];
					mesh->AddVertex(vertex);

					// エッジ平滑化用のデータを追加
					if (smoothing) {
						// vキー（座標データ）の番号と、全て合成した頂点のインデックスをセットで登録する
						mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
					}
				}
				else {
					char c;
					index_stream >> c;
					// スラッシュ2連続の場合、頂点番号のみ
					if (c == '/') {
						// 頂点データの追加
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = { 0, 0, 1 };
						vertex.uv = { 0, 0 };
						mesh->AddVertex(vertex);

						//// エッジ平滑化用のデータを追加
						//if (smoothing) {
						//	// vキー（座標データ）の番号と、全て合成した頂点のインデックスをセットで登録する
						//	AddSmoothData(indexPosition, (unsigned short)GetVertexCount() - 1);
						//}
					}
					else {
						index_stream.seekg(-1, ios_base::cur); // 1文字戻る
						index_stream >> indexTexcoord;
						index_stream.seekg(1, ios_base::cur); // スラッシュを飛ばす
						index_stream >> indexNormal;
						// 頂点データの追加
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = normals[indexNormal - 1];
						vertex.uv = { 0, 0 };
						mesh->AddVertex(vertex);

						// エッジ平滑化用のデータを追加
						if (smoothing) {
							// vキー（座標データ）の番号と、全て合成した頂点のインデックスをセットで登録する
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
						}
					}
				}
				// インデックスデータの追加
				if (faceIndexCount >= 3) {
					// 四角形ポリゴンの4点目なので、
					// 四角形の0,1,2,3の内 2,3,0で三角形を構築する
					mesh->AddIndex(indexCountTex - 1);
					mesh->AddIndex(indexCountTex);
					mesh->AddIndex(indexCountTex - 3);
				}
				else {
					mesh->AddIndex(indexCountTex);
				}
				indexCountTex++;
				faceIndexCount++;
				//// インデックスデータの追加
				//indices.emplace_back((unsigned short)indices.size());
			}
		}
	}
	// ファイルを閉じる
	file.close();

	// 頂点法線の平均によるエッジの平滑化
	if (smoothing) {
		mesh->CalculateSmoothedVertexNormals();
	}
}

void Model::CreateBuffers()
{
	// メッシュのバッファ生成
	for (auto& m : meshes) {
		m->CreateBuffers();
	}

	// マテリアルの数値を定数バッファに反映
	for (auto& m : materials) {
		m.second->Update();
	}

}

void Model::AddMaterial(Material* material)
{
	// コンテナに登録
	materials.emplace(material->name, material);
}

void Model::SetDevice(ID3D12Device* device)
{
	Model::device = device;
	Mesh::SetDevice(device);
	Material::StaticInitialize(device);
}