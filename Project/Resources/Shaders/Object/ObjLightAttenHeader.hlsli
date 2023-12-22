cbuffer WorldTransform : register(b0)
{
	float4 color; // �F�iRGBA�j
	matrix world; // �R�c�ϊ��s��
    uint bloomActive;
};

cbuffer ViewProjection : register(b1)
{
	matrix view;       // �r���[�ϊ��s��
	matrix projection; // �v���W�F�N�V�����ϊ��s��
	float3 cameraPos;  // �J�������W�i���[���h���W
}

cbuffer cbuff1:register(b2)
{
	float3 m_ambient;//:packoffset(c0);// �A���r�G���g�W��
	float3 m_diffuse;//:packoffset(c1);// �f�B�t���[�Y�W��
	float3 m_specular;//:packoffset(c2);// �X�y�L�����[�W��
	float m_alpha;//: packoffset(c2.w);// �A���t�@
}

// ���s�����̐�
static const int DIRLIGHT_NUM = 3;

struct DirLight
{
	float3 lightv;    // ���C�g�ւ̕����̒P�ʃx�N�g��
	float3 lightcolor;    // ���C�g�̐F(RGB)
	uint active;
};

// �_�����̐�
static const int POINTLIGHT_NUM = 15;

struct PointLight
{
	float3 lightpos;    // ���C�g���W
	float3 lightcolor;  // ���C�g�̐F(RGB)
	float3 lightatten;	// ���C�g���������W��
    float lightIndensity;
    float lightRadius;
    float lightDecay;
	uint active;
};

// �X�|�b�g���C�g�̐�
static const int SPOTLIGHT_NUM = 3;

struct SpotLight
{
	float3 lightv;		// ���C�g�̌��������̋t�x�N�g���i�P�ʃx�N�g���j
	float3 lightpos;    // ���C�g���W
	float3 lightcolor;  // ���C�g�̐F(RGB)
	float3 lightatten;	// ���C�g���������W��
	float2 lightfactoranglecos; // ���C�g�����p�x�̃R�T�C��
	uint active;
};

// �ۉe�̐�
static const int CIRCLESHADOW_NUM = 3;

struct CircleShadow
{
	float3 dir;		// ���e�����̋t�x�N�g���i�P�ʃx�N�g���j
	float3 casterPos;    // �L���X�^�[���W
	float  distanceCasterLight;	// �L���X�^�[�ƃ��C�g�̋���
	float3 atten;	// ���������W��
	float2 factorAngleCos; // �����p�x�̃R�T�C��
	uint active;
};

// ���C�g�O���[�v
cbuffer LightGroup : register(b3)
{
	float3 ambientColor;
	float3 diffuseColor;// �f�B�t���[�Y�J���[
	float3 specularColor;// �X�y�L�����[�J���[
	
	DirLight dirLights[DIRLIGHT_NUM];
	PointLight pointLights[POINTLIGHT_NUM];
	SpotLight spotLights[SPOTLIGHT_NUM];
	CircleShadow circleShadows[CIRCLESHADOW_NUM];
}

// �t�H�O
cbuffer Fog : register(b4)
{
	float4 fogColor;
	bool isActiveFog;
	float nearFog;
	float farFog;
}

// �f�B�]���u
cbuffer Dissolve : register(b5)
{
    float4 dissolveColor;
    bool isActiveDissolve;
    float dissolvePower;
    float dissolveTime;
    float dissolveSmoothMin;
}

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
	float4 svpos : SV_POSITION; // �V�X�e���p���_���W
	float4 worldpos : POSITION;	//���[���h���W
	float3 normal :NORMAL; // �@���x�N�g��
	float2 uv  :TEXCOORD; // uv�l
};