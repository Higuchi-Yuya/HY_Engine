cbuffer WorldTransform : register(b0)
{
    float4 color; // �F�iRGBA�j
    matrix world; // �R�c�ϊ��s��
};

cbuffer ViewProjection : register(b1)
{
    matrix view; // �r���[�ϊ��s��
    matrix projection; // �v���W�F�N�V�����ϊ��s��
    float3 cameraPos; // �J�������W�i���[���h���W
}

// ���_�V�F�[�_�[����s�N�Z���V�F�[�_�[�ւ̂����Ɏg�p����\����
struct VSOutput
{
	float4 svpos : SV_POSITION; // �V�X�e���p���_���W
	float4 worldpos : POSITION;	//���[���h���W
	float3 normal :NORMAL; // �@���x�N�g��
	float2 uv  :TEXCOORD; // uv�l
};