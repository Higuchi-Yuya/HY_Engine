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

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
	float4 svpos : SV_POSITION; // �V�X�e���p���_���W
	float4 worldpos : POSITION;	//���[���h���W
	float3 normal :NORMAL; // �@���x�N�g��
	float2 uv  :TEXCOORD; // uv�l
};

struct PSOutput
{
    float4 ColorMap : SV_Target0;
    float4 WorldPosMap : SV_Target1;
    float4 NomalMap : SV_Target2;
    float4 AmbientMap : SV_Target3;
    float4 DiffuseMap : SV_Target4;
    float4 SpecularMap : SV_Target5;
};