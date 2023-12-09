cbuffer cbuff0 : register(b0)
{
	float2 centerPos; // ���S���W
	bool isActive;    // �g����I���ɂ��邩�ǂ���
    float alpha;      // �P�x�F0�`1
    float waveSpan;   // �g�̐� �����قǍׂ����F0�`100
    float waveFrame;  // ���S���W����L�����Ă������x�F0�`1
    float waveScale;  // �g�̊Ԋu�F0�`0.1f 0.1�ȏゾ�Ƒ傫���Ȃ肷����
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