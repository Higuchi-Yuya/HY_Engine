cbuffer cbuff0 : register(b0)
{
	float4 vignetteColor; // �F(RGBA)
	float vignetteRadius; // �L��
    float vignetteFeather;
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