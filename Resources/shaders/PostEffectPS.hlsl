#include "PostEffectHeader.hlsli"

Texture2D<float4> tex0 : register(t0);  // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex1 : register(t1); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float4 main(VSOutput input) : SV_TARGET
{
    float2 offset = { 0.5f, 0.0f };
    
    float4 colorTex0 = tex0.Sample(smp, input.uv);
    float4 colorTex1 = tex1.Sample(smp, input.uv);
    
    float4 color = colorTex0;
    if (fmod(input.uv.y, 0.1f) < 0.05f)
    {
        color = colorTex1;
    }
    
    return float4(color.rgb, 1);
}