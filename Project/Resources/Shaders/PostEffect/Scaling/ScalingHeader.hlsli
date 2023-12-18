cbuffer cbuff0 : register(b0)
{
    float2 scalingTiling; // �^�C�����O
    float2 scalingOffset; // �I�t�Z�b�g
	bool isActive;        // �X�P�[�����O���I���ɂ��邩�ǂ���
    float alpha;          // �����_�[�e�N�X�`���̃A���t�@�l�F0�`1
};

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
	float4 svpos : SV_POSITION; // �V�X�e���p���_���W
	float2 uv  :TEXCOORD; // uv�l
};

struct PSOutput
{
    float4 target0 : SV_TARGET0;
};