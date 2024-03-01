#include "DeferredObjHeader.hlsli"

Texture2D<float4> tex : register(t0);  // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

PSOutput main(VSOutput input) : SV_TARGET
{	
	// �e�N�X�`���}�b�s���O
    PSOutput output;
    float4 texColor = tex.Sample(smp, input.uv);
    float4 baseColor = color * texColor;
    
    output.ColorMap = baseColor;
    output.WorldPosMap = float4(input.worldpos.x, input.worldpos.y, input.worldpos.z, 1.0f);
    output.NomalMap = float4(input.normal.x, input.normal.y, input.normal.z, 1.0f);
    output.AmbientMap = float4(m_ambient, 1.0f);
    output.DiffuseMap = float4(m_diffuse, 1.0f);
    output.SpecularMap = float4(m_specular, 1.0f);
    
	// �e�N�X�`���̐F�͎g�킸�F�𐔒l�w��
	return output;

}