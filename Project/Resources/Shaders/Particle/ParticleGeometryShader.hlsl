#include "ParticleShaderHeader.hlsli"

// �l�p�`�̒��_��
static const uint vnum = 4;

// �Z���^�[����̃I�t�Z�b�g
static const float4 offset_array[vnum] =
{
	float4(-0.5f,-0.5f,0,0), // ����
	float4(-0.5f,+0.5f,0,0), // ����
	float4(+0.5f,-0.5f,0,0), // �E��
	float4(+0.5f,+0.5f,0,0), // �E��
};

static const float2 uv_array[vnum] =
{
	float2(0,1),
	float2(0,0),
	float2(1,1),
	float2(1,0)
};

// �O�p�`�̓��͂���A�_���O�o�͂���T���v��
[maxvertexcount(vnum)]
void main(
	point VSOutput input[1] : SV_POSITION,
	// �_�X�g���[��
	inout TriangleStream< GSOutput > output
)
{
    GSOutput element = (GSOutput)0; // �o�͒��_�f�[�^
	for (uint i = 0; i < vnum; i++) {
		// ���S����̃I�t�Z�b�g���r���{�[�h��]�i���f�����W�j
		float4 offset;
		//offset = mul(matBillboard, offset_array[i]);
		// ���S����I�t�Z�b�g���X�P�[�����O
		offset = offset_array[i] * input[0].scale;

		// ���S����̃I�t�Z�b�g���r���{�[�h��]�i���f�����W�j
		offset = mul(matBillboard, offset);

		// ���[���h���W�x�[�X�ŁA���炷
		element.svpos = input[0].pos + offset;// ���_���W���R�s�[
		
		// �r���[�A�ˉe�ϊ�
		element.svpos = mul(mat, element.svpos);
		element.uv = uv_array[i];
        element.color = input[0].color;
		output.Append(element);
	}

}