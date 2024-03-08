// ���s�����̐�
static const int DIRLIGHT_NUM = 3;

struct DirLight
{
    float3 lightv; // ���C�g�ւ̕����̒P�ʃx�N�g��
    float3 lightcolor; // ���C�g�̐F(RGB)
    uint active;
};

// �_�����̐�
static const int POINTLIGHT_NUM = 30;

struct PointLight
{
    float3 lightpos; // ���C�g���W
    float3 lightcolor; // ���C�g�̐F(RGB)
    float3 lightatten; // ���C�g���������W��
    float lightIndensity;
    float lightRadius;
    float lightDecay;
    uint active;
};

// �X�|�b�g���C�g�̐�
static const int SPOTLIGHT_NUM = 3;

struct SpotLight
{
    float3 lightv; // ���C�g�̌��������̋t�x�N�g���i�P�ʃx�N�g���j
    float3 lightpos; // ���C�g���W
    float3 lightcolor; // ���C�g�̐F(RGB)
    float3 lightatten; // ���C�g���������W��
    float2 lightfactoranglecos; // ���C�g�����p�x�̃R�T�C��
    float lightIndensity; // ���C�g�̋P�x
    uint active;
};

// �ۉe�̐�
static const int CIRCLESHADOW_NUM = 3;

struct CircleShadow
{
    float3 dir; // ���e�����̋t�x�N�g���i�P�ʃx�N�g���j
    float3 casterPos; // �L���X�^�[���W
    float distanceCasterLight; // �L���X�^�[�ƃ��C�g�̋���
    float3 atten; // ���������W��
    float2 factorAngleCos; // �����p�x�̃R�T�C��
    uint active;
};

// ���C�g�O���[�v
cbuffer LightGroup : register(b0)
{	
    DirLight dirLights[DIRLIGHT_NUM];
    PointLight pointLights[POINTLIGHT_NUM];
    SpotLight spotLights[SPOTLIGHT_NUM];
    CircleShadow circleShadows[CIRCLESHADOW_NUM];
}

// �t�H�O
cbuffer Fog : register(b1)
{
    float4 fogColor;
    bool isActiveFog;
    float nearFog;
    float farFog;
}

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
    float4 svpos : SV_POSITION; // �V�X�e���p���_���W
    float2 uv : TEXCOORD; // uv�l
};