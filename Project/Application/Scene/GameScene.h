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
#include "Dissolve.h"
#include "ParticleManager.h"

#include "GameCamera.h"
#include "GameCollider.h"
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

	void DrawParticle();

	void Draw2DFront();

private:// �T�u�N���X
	// �V�[���N���X�i���j
	enum class Scene {
		Title,
		Game,
		Result,
	};

private:// �v���C�x�[�g�֐�
	/// <summary>
	/// �^�C�g���V�[���̍X�V����
	/// </summary>
	void TitleUpdate();

	/// <summary>
	/// �Q�[���V�[���̍X�V����
	/// </summary>
	void GameSceneUpdate();

	/// <summary>
	/// �V�[���`�F���W�̍X�V����
	/// </summary>
	void SceneChageUpdate();

private:// �����o�ϐ�

	// ����
	std::unique_ptr<Input> input_ = nullptr;

#pragma region �����֘A
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
	Vector3 circleShadowCasterPos = { 0,0,0 };
	Vector3 circleShadowDir = { 0,-1,0 };
	Vector3 circleShadowAtten = { 1.5f,0.6f,0.0f };
	Vector2 circleShadowFactorAngle = { 0.0f,0.5f };
	bool isActiveCircleShadow = true;

	Vector3 AmColor = { 1,0,0 };
	Vector3 DiColor = { 0,1,0 };
	Vector3 SpColor = { 0,0,1 };

	// �t�H�O
	std::unique_ptr<Fog> fog = nullptr;
	bool isFogActive = true;
#pragma endregion


#pragma region �e�N�X�`���n���h��

	std::unique_ptr<Texture> textureHandleDefu;
	std::unique_ptr<Texture> titleFontTexHandle;
	std::unique_ptr<Texture> titleBackTexHandle;
	std::unique_ptr<Texture> titleButtonTexHandle;

	// �t�F�[�h�C���t�F�[�h�A�E�g�p�̉摜�n���h��
	std::unique_ptr<Texture> blackOutTexHandle_;
#pragma endregion


#pragma region �X�v���C�g

	std::unique_ptr<Sprite> spriteProvisional = nullptr;
	std::unique_ptr<Sprite> titleKariFont = nullptr;
	std::unique_ptr<Sprite> titleKariBack = nullptr;
	std::unique_ptr<Sprite> titleKariPressA = nullptr;
	Vector2 spritePos;

	// �t�F�[�h�C���t�F�[�h�A�E�g�p�̉摜�X�v���C�g
	std::unique_ptr<Sprite> blackOut = nullptr;
#pragma endregion


#pragma region ���f��

	std::unique_ptr<Model> playerModel_ = nullptr;
	std::unique_ptr<Model> modelMedama_ = nullptr;

	std::unique_ptr<Model> modelSkydome = nullptr;
	std::unique_ptr<Model> modelGround = nullptr;

#pragma endregion


#pragma region �I�u�W�F�N�g�֘A
	std::unique_ptr<Object3d> objSkydome = nullptr;
	std::unique_ptr<Object3d> objGround = nullptr;

	std::unique_ptr<Object3d>objMedama_;
	bool isDissolve = false;
#pragma endregion


#pragma region �J�����֘A
	// �r���[�v���W�F�N�V����
	std::unique_ptr<GameCamera>gameCamera;
#pragma endregion


#pragma region ���֘A
	Sound sound;
	bool isActiveSound = false;
	bool isStopSound = false;
#pragma endregion


#pragma region �����蔻��
	std::unique_ptr < GameCollider> gameCollider;
#pragma endregion


#pragma region FBX���f���̊m�F
	//std::unique_ptr<FbxModel> fbxmodel_;
	WorldTransform fbxTrans_;
	//std::unique_ptr<FbxAnimetion> modelAnim_;
	DirectXCommon* dxCommon_ = nullptr;

	float frem = 0;
	uint32_t BoneNum = 0;
#pragma endregion


#pragma region �v���C���[�֘A
	std::unique_ptr<Player> player_;

#pragma endregion


#pragma region �G�l�~�[�֘A
	std::vector<Enemy*>enemys_;
	std::unique_ptr<Enemy> enemy_;
#pragma endregion


#pragma region �V�[���`�F���W�֘A
	// �V�[���Ǘ�
	Scene scene = Scene::Title;

	// �u���b�N�A�E�g�̕ϐ�
	float blackAlpha = 0.0f;
	bool sceneChangeFlag = false;
	Scene oldScene = Scene::Title;
	bool resultChange = false;
#pragma endregion


#pragma region ���x���f�[�^�֘A
	std::unique_ptr<LevelData> levelData_;

	std::map<std::string, Model*> models;
	std::vector<Object3d*> objects;
	
#pragma endregion

};

