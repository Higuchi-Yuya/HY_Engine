#include "SilhouetteHeader.hlsli"

Texture2D<float4> tex : register(t0);  // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{	
	// �e�N�X�`���}�b�s���O
	
    float2 tilling = { 1, 1 };
    float2 offset = { 0, 0 };
    float4 texcolor = tex.Sample(smp, input.uv * tilling + offset);

    float4 color = { 0.17f, 0.12f, 0.08f, 1 };
    
    return color;

}