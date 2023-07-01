#pragma once
#include "Sprite.h"
#include "Sound.h"
#include "Object3d.h"
#include "ViewProjection.h"
#include "Model.h"
#include "Quaternion.h"
#include "LightGroup.h"
#include "ImGuiManager.h"
#include "CollisionPrimitive.h"
#include "FbxLoader.h"
#include "FbxAnimetion.h"
#include "LevelLoder.h"
#include "TextureManager.h"
#include "Player.h"
#include "Enemy.h"

#include "GameCamera.h"

#include <vector>

class CollisionManager;
class Player;
class TouchableObject;

class GameScene
{
public:// �����o�֐�
	
	// �f�X�g���N�^
	~GameScene();

	// ����������
	void Initialize();

	// �X�V����
	void Update();

	// Imgui�̍X�V����
	void ImguiUpdate();

	void SetDxComon(DirectXCommon* dxCommon) { dxCommon_ = dxCommon; }

	// �`�揈��
	void Draw2DBack();

	void Draw3D();

	void Draw2DFront();

private:// �����o�ϐ�

	// ����
	std::unique_ptr<Input> input_ = nullptr;

	// �I�u�W�F�N�g���ʂ̃��C�g�̏�����
	std::unique_ptr<LightGroup> light = nullptr;
	
	// ���s����
	bool isActiveDirectional = true;

	// �_�����̃X�e�[�^�X
	float pointLightPos[3] = { 0,1,0 };
	float pointLightColor[3] = { 1,1,1 };
	float pointLightAtten[3] = { 0.3f,0.1f,0.1f };
	bool isActivePoint = false;

	// �X�|�b�g���C�g�̃X�e�[�^�X
	Vector3 spotLightDir = { 0,-1,0 };
	Vector3 spotLightPos = { 0,5,0 };
	Vector3 spotLightColor = { 1,1,1 };
	Vector3 spotLightAtten = { 0.0f,0.0f,0.0f };
	Vector2 spotLightFactorAngle = { 20.0f,30.0f };
	bool isActiveSpot = false;

	// �ۉe�̃X�e�[�^�X
	Vector3 circleShadowDir = { 0,-1,0 };
	Vector3 circleShadowAtten = { 1.5f,0.6f,0.0f };
	Vector2 circleShadowFactorAngle = { 0.0f,0.5f };
	bool isActiveCircleShadow = true;

	Vector3 fighterPos = { 1,0.0f,0 };

	Vector3 AmColor={1,0,0};
	Vector3 DiColor={ 0,1,0 };
	Vector3 SpColor={ 0,0,1 };

	// �e�N�X�`���n���h��
	std::unique_ptr<Texture> textureHandle;
	std::unique_ptr<Texture> textureHandle2;

	// �X�v���C�g
	std::unique_ptr<Sprite> spriteBack_ = nullptr;
	std::unique_ptr<Sprite> sprite2 = nullptr;

	// ���f��
	//Model* model = nullptr;
	std::unique_ptr<Model> model_2 = nullptr;
	
	std::unique_ptr<Model> modelMedama_ = nullptr;
	//Model* groundModel = nullptr;

	//Model* modelPlane = nullptr;
	//Model* modelBox = nullptr;
	//Model* modelPyramid = nullptr;

	// �I�u�W�F�N�g
	//Object3d* object3d = nullptr;
	//std::unique_ptr<Object3d> objMedama = nullptr;

	//Object3d* point1 = nullptr;
	//Object3d* point2 = nullptr;
	//Object3d* point3 = nullptr;

	//Object3d* rayobj = nullptr;

	//Player* objFighter = nullptr;
	//TouchableObject* groundObj = nullptr;
	//TouchableObject* groundObj2 = nullptr;

	//std::vector<Object3d*>objects;
	
	// �r���[�v���W�F�N�V����
	std::unique_ptr<GameCamera>gameCamera;



	Vector2 spritePos;
	char buf[256] = "";
	float f = 0.0f;

	std::unique_ptr<Fog> fog = nullptr;
	bool isFogActive = true;

	Sound sound;
	bool isActiveSound = false;
	bool isStopSound = false;

#pragma region �����蔻��
	CollisionManager* collisionManager = nullptr;

#pragma endregion

#pragma region ���[�_�[�̊m�F
	LevelData* levelData = nullptr;

	std::unique_ptr<Model> modelSkydome = nullptr;
	std::unique_ptr<Model> modelGround = nullptr;
	//Model* modelFighter = nullptr;
	//Model* modelSphere = nullptr;

	std::unique_ptr<Object3d> objSkydome = nullptr;
	std::unique_ptr<Object3d> objGround = nullptr;
	//Object3d* objFighter = nullptr;
	//Object3d* objSphere = nullptr;

	//std::map<std::string, Model*> models;
	//std::vector<Object3d*> objects;
#pragma endregion

#pragma region FBX���f���̊m�F
	std::unique_ptr<FbxModel> fbxmodel_;
	WorldTransform fbxTrans_;
	std::unique_ptr<FbxAnimetion> modelAnim_;
	DirectXCommon* dxCommon_ = nullptr;

	float frem = 0;
	uint32_t BoneNum = 0;
#pragma endregion


#pragma region �v���g�^�C�v�p�ϐ�
	std::unique_ptr<Player> player_;
	std::unique_ptr<Enemy> enemy_;

#pragma endregion

};

