#include "OBJShaderHeader.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	// �@���Ƀ��[���h�s��ɂ��X�P�[�����O�E��]��K�p
	// ���X�P�[�����O����l�ȏꍇ�̂ݐ�����
	float4 wnormal = normalize(mul(world, float4(normal, 0)));
	float4 wpos = mul(world, pos);

	//// �E�A���A���̕��������������C�g
	//float3 lightv = float3(1, 0, 0);
	//lightv = normalize(lightv);
	// ���C�g�̐F�i���j
	//float3 lightcolor = float3(1, 1, 1);

	VSOutput output; // �s�N�Z���V�F�[�_�[�ɓn���l
	output.svpos = mul(mul(mul(projection, view), world), pos);

	// �����ˌ� 
	float3 ambient = m_ambient;
	// �g�U���ˌ�
	float3 diffuse = dot(lightv, wnormal.xyz) * m_diffuse;
	//// ���_���W
	//const float3 eye = float3(0, 0, -20);
	// ����x
	const float shininess = 4.0f;
	// ���_���王�_�ւ̕����x�N�g��
	float3 eyedir = normalize(cameraPos - wpos.xyz);
	// ���ˌ��x�N�g��
	float3 reflect = normalize(-lightv + 2 * dot(lightv, wnormal.xyz) * wnormal.xyz);
	// ���ʔ��ˌ�
	float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

	output.worldpos = wpos;
	// �S�ĉ��Z����
	output.color.rgb = (ambient + diffuse + specular) * lightcolor;
	output.color.a = m_alpha;
	//output.normal = wnormal.xyz;
	output.uv = uv;

	return output;
}