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

ID3D12Device* Model::sDevice_ = nullptr;

Model::Model()
{
}

Model::~Model()
{
	
	for (auto m : materials_) {
		delete m.second;
	}
	// �S���b�V�����폜
	for (auto mesh : meshes_) {
		delete mesh;
	}
	materials_.clear();
}

Model* Model::LoadFromOBJ(const std::string& modelname, bool smoothing)
{
	Model* model = new Model;

	// �I�u�W�F�N�g�t�@�C������f�[�^��ǂݍ���
	model->LoadFromOBJInternal(modelname, smoothing);

	// �o�b�t�@����
	model->CreateBuffers();

	return model;
}

void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename)
{
	//�t�@�C���X�g���[��
	std::ifstream file;
	// �}�e���A���t�@�C�����J��
	file.open(directoryPath + filename);
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	Material* material = nullptr;

	// 1�s���ǂݍ���
	string line;
	while (getline(file, line)) {
		// ��s���̕�������X�g���[���ɕϊ�
		std::istringstream line_stream(line);
		// ���p�X�y�[�X��؂�ōs�̐擪��������擾
		string key;
		getline(line_stream, key, ' ');

		// �擪�̃^�u�����͖�������
		if (key[0] == '\t') {
			key.erase(key.begin());// �擪�̕������폜
		}

		// �擪������newmtl�Ȃ�}�e���A����
		if (key == "newmtl") {
			// ���Ƀ}�e���A���������
			if (material) {
				// �}�e���A�����R���e�i�ɓo�^
				AddMaterial(material);
			}

			// �V�����}�e���A���𐶐�
			material = Material::Create();

			// �}�e���A�����ǂݍ���
			line_stream >> material->name;
		}

		// �擪������Ks�Ȃ�A���r�G���g�F
		if (key == "Ka") {
			line_stream >> material->ambient.x;
			line_stream >> material->ambient.y;
			line_stream >> material->ambient.z;

		}
		// �擪������Ks�Ȃ�f�B�t���[�Y�F
		if (key == "Kd") {
			line_stream >> material->diffuse.x;
			line_stream >> material->diffuse.y;
			line_stream >> material->diffuse.z;

		}

		// �擪������Ks�Ȃ�X�y�L�����[�F
		if (key == "Ks") {
			line_stream >> material->specular.x;
			line_stream >> material->specular.y;
			line_stream >> material->specular.z;

		}
		// �擪������map_Kd�Ȃ�e�N�X�`���t�@�C����
		if (key == "map_Kd") {
			// �e�N�X�`���̃t�@�C�����ǂݍ���
			line_stream >> material->textureFilename;
			// �e�N�X�`���ǂݍ���
			material->LoadTexture(directoryPath, material->textureFilename);
		}
	}
	// �t�@�C�������
	file.close();

	if (material) {
		// �}�e���A����o�^
		AddMaterial(material);
	}
}

void Model::Draw(ID3D12GraphicsCommandList* cmdList)
{
	// nullptr�`�F�b�N
	assert(sDevice_);
	assert(cmdList);

	// �S���b�V����`��
	for (auto& mesh : meshes_) {
		mesh->Draw(cmdList);
	}
}



void Model::LoadFromOBJInternal(const std::string& modelname, bool smoothing)
{
	// �t�@�C���X�g���[��
	std::ifstream file;

	// obj�t�@�C�����J��
	//const string modelname = "skydome";
	const string filename = modelname + ".obj"; // "triangle_mat.obj"
	const string directoryPath = "Resources/3D_Resources/Object_File/" + modelname + "/"; // "Resources/triangle_mat/"
	file.open(directoryPath + filename); //"Resources/triangle_mat/triangle_mat.obj"
	// �t�@�C���I�[�v�����s���`�F�b�N
	if (file.fail()) {
		assert(0);
	}

	name_ = modelname;

	// ���b�V������
	meshes_.emplace_back(new Mesh);
	Mesh* mesh = meshes_.back();
	int indexCountTex = 0;
	int indexCountNoTex = 0;

	vector<Vector3> positions; // ���_���W
	vector<Vector3> normals; // �@���x�N�g��
	vector<Vector2> texcoords; // �e�N�X�`��UV
	// 1�s���ǂݍ���
	string line;
	while (getline(file, line)) {
		// 1�s���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
		std::istringstream line_stream(line);

		// ���p�X�y�[�X��؂�ōs�̐擪��������擾
		string key;
		getline(line_stream, key, ' ');

		// �擪������mtllib�Ȃ�}�e���A��
		if (key == "mtllib")
		{
			// �}�e���A���̃t�@�C�����ǂݍ���
			string filename;
			line_stream >> filename;

			// �}�e���A���ǂݍ���
			LoadMaterial(directoryPath, filename);
		}
		// �擪������g�Ȃ�O���[�v�̊J�n
		if (key == "g") {

			// �J�����g���b�V���̏�񂪑����Ă���Ȃ�
			if (mesh->GetName().size() > 0 && mesh->GetVertexCount() > 0) {
				// ���_�@���̕��ςɂ��G�b�W�̕�����
				if (smoothing) {
					mesh->CalculateSmoothedVertexNormals();
				}
				// ���̃��b�V������
				meshes_.emplace_back(new Mesh);
				mesh = meshes_.back();
				indexCountTex = 0;
			}

			// �O���[�v���ǂݍ���
			string groupName;
			line_stream >> groupName;

			// ���b�V���ɖ��O���Z�b�g
			mesh->SetName(groupName);
		}
		// �擪������v�Ȃ璸�_���W
		if (key == "v") {
			// X,Y,Z���W�ǂݍ���
			Vector3 position{};
			line_stream >> position.x;
			line_stream >> position.y;
			line_stream >> position.z;

			// ���W�f�[�^�ɒǉ�
			positions.emplace_back(position);
		}

		// �擪������vt�Ȃ�e�N�X�`��
		if (key == "vt") {
			// U,V�����ǂݍ���
			Vector2 texcoord{};
			line_stream >> texcoord.x;
			line_stream >> texcoord.y;

			// V�������]
			texcoord.y = 1.0f - texcoord.y;
			// �e�N�X�`�����W�f�[�^�ɒǉ�
			texcoords.emplace_back(texcoord);

		}

		// �擪������VN�Ȃ�@���x�N�g��
		if (key == "vn") {
			// X,Y,Z�����ǂݍ���
			Vector3 normal{};
			line_stream >> normal.x;
			line_stream >> normal.y;
			line_stream >> normal.z;
			// �@���x�N�g���f�[�^�ɒǉ�
			normals.emplace_back(normal);
		}
		// �擪������usemtl�Ȃ�}�e���A�������蓖�Ă�
		if (key == "usemtl") {
			if (mesh->GetMaterial() == nullptr) {
				// �}�e���A���̖��ǂݍ���
				string materialName;
				line_stream >> materialName;

				// �}�e���A�����Ō������A�}�e���A�������蓖�Ă�
				auto itr = materials_.find(materialName);
				if (itr != materials_.end()) {
					mesh->SetMaterial(itr->second);
				}
			}
		}
		// �擪������f�Ȃ�|���S���i�O�p�`�j
		if (key == "f") {
			int faceIndexCount = 0;
			// ���p�X�y�[�X��؂�ōs�̑�����ǂݍ���
			string index_string;
			while (getline(line_stream, index_string, ' ')) {
				// ���_�C���f�b�N�X�P���̕�������X�g���[���ɕϊ����ĉ�͂��₷������
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				// ���_�ԍ�
				index_stream >> indexPosition;

				Material* material = mesh->GetMaterial();
				index_stream.seekg(1, ios_base::cur);// �X���b�V�����΂�
				// �}�e���A���A�e�N�X�`��������ꍇ
				if (materials_.size() > 0) {
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur);// �X���b�V�����΂�
					index_stream >> indexNormal;
					// ���_�f�[�^�̒ǉ�
					Mesh::VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];
					mesh->AddVertex(vertex);

					// �G�b�W�������p�̃f�[�^��ǉ�
					if (smoothing) {
						// v�L�[�i���W�f�[�^�j�̔ԍ��ƁA�S�č����������_�̃C���f�b�N�X���Z�b�g�œo�^����
						mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
					}
					AddMinMaxVertex(vertex.pos);
				}
				else {
					char c;
					index_stream >> c;
					// �X���b�V��2�A���̏ꍇ�A���_�ԍ��̂�
					if (c == '/') {
						// ���_�f�[�^�̒ǉ�
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = { 0, 0, 1 };
						vertex.uv = { 0, 0 };
						mesh->AddVertex(vertex);

						//// �G�b�W�������p�̃f�[�^��ǉ�
						//if (smoothing) {
						//	// v�L�[�i���W�f�[�^�j�̔ԍ��ƁA�S�č����������_�̃C���f�b�N�X���Z�b�g�œo�^����
						//	AddSmoothData(indexPosition, (unsigned short)GetVertexCount() - 1);
						//}

						AddMinMaxVertex(vertex.pos);
					}
					else {
						index_stream.seekg(-1, ios_base::cur); // 1�����߂�
						index_stream >> indexTexcoord;
						index_stream.seekg(1, ios_base::cur); // �X���b�V�����΂�
						index_stream >> indexNormal;
						// ���_�f�[�^�̒ǉ�
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = normals[indexNormal - 1];
						vertex.uv = { 0, 0 };
						mesh->AddVertex(vertex);

						// �G�b�W�������p�̃f�[�^��ǉ�
						if (smoothing) {
							// v�L�[�i���W�f�[�^�j�̔ԍ��ƁA�S�č����������_�̃C���f�b�N�X���Z�b�g�œo�^����
							mesh->AddSmoothData(indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
						}
						AddMinMaxVertex(vertex.pos);
					}
				}
				// �C���f�b�N�X�f�[�^�̒ǉ�
				if (faceIndexCount >= 3) {
					// �l�p�`�|���S����4�_�ڂȂ̂ŁA
					// �l�p�`��0,1,2,3�̓� 2,3,0�ŎO�p�`���\�z����
					mesh->AddIndex(indexCountTex - 1);
					mesh->AddIndex(indexCountTex);
					mesh->AddIndex(indexCountTex - 3);
				}
				else {
					mesh->AddIndex(indexCountTex);
				}
				
				indexCountTex++;
				faceIndexCount++;
				//// �C���f�b�N�X�f�[�^�̒ǉ�
				//indices.emplace_back((unsigned short)indices.size());
			}
		}
	}
	// �t�@�C�������
	file.close();

	// ��x�����ŏ��̒��_��o�^����t���O���I�t
	isFirstAddMinMax = false;

	// ���_�@���̕��ςɂ��G�b�W�̕�����
	if (smoothing) {
		mesh->CalculateSmoothedVertexNormals();
	}
}

void Model::CreateBuffers()
{
	// ���b�V���̃o�b�t�@����
	for (auto& m : meshes_) {
		m->CreateBuffers();
	}

	// �}�e���A���̐��l��萔�o�b�t�@�ɔ��f
	for (auto& m : materials_) {
		m.second->Update();
	}

}

void Model::AddMaterial(Material* material)
{
	// �R���e�i�ɓo�^
	materials_.emplace(material->name, material);
}

void Model::AddMinMaxVertex(Vector3 vertexPos)
{
	if (isFirstAddMinMax == false) {
		minVertex_ = vertexPos;
		maxVertex_ = vertexPos;
		isFirstAddMinMax = true;
	}

	// ���������̂���

	// x
	if (vertexPos.x <= minVertex_.x) {
		minVertex_.x = vertexPos.x;
	}
	// y
	if (vertexPos.y < minVertex_.y) {
		minVertex_.y = vertexPos.y;
	}
	// z
	if (vertexPos.z <= minVertex_.z) {
		minVertex_.z = vertexPos.z;
	}

	// �傫�����̂���

	// x
	if (vertexPos.x >= maxVertex_.x) {
		maxVertex_.x = vertexPos.x;
	}
	// y
	if (vertexPos.y >= maxVertex_.y) {
		maxVertex_.y = vertexPos.y;
	}
	// z
	if (vertexPos.z >= maxVertex_.z) {
		maxVertex_.z = vertexPos.z;
	}
}

void Model::SetDevice(ID3D12Device* device)
{
	Model::sDevice_ = device;
	Mesh::SetDevice(device);
	Material::StaticInitialize(device);
}