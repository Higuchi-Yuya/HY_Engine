#include "CompositionHeader.hlsli"

Texture2D<float4> tex0 : register(t0); // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1); // 1番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
    float2 tiling;
    float2 offset;
    
    //if (distance(input.uv, float2(0.5f, 0.5f)) > 0.5f)
    //{
    //}
    
    tiling = compoTiling;
    offset = float2(
    (1 - tiling.x) * 10 * 0.05f,
    (1 - tiling.y) * 10 * 0.05f);
    
    float4 texcolor0 = tex0.Sample(smp, input.uv * tiling + offset);
    float4 texcolor1 = tex1.Sample(smp, input.uv * tiling + offset);
    
    float4 result = float4(texcolor0.rgb + texcolor1.rgb, 1);
    
    return result;
}