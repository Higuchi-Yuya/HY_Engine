#include "DeferredObjHeader.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	// �@���Ƀ��[���h�s��ɂ��X�P�[�����O�E��]��K�p
	// ���X�P�[�����O����l�ȏꍇ�̂ݐ�����
    float4 wpos = mul(world, pos);
	float4 wnormal = normalize(mul(world, float4(normal, 0)));

	VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
	output.worldpos = wpos;
	output.svpos = mul(mul(mul(projection, view), world), pos);
	output.normal = wnormal.xyz;
	output.uv = uv;

	return output;
}