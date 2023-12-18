#include "ScalingHeader.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー


float4 main(VSOutput input) : SV_TARGET
{
    float2 tiling;
    float2 offset;
    

    tiling = scalingTiling;
    offset = float2(
                (1 - tiling.x) * 10 * 0.05f,
                (1 - tiling.y) * 10 * 0.05f);
    
    
    float4 texColor = tex.Sample(smp, (input.uv + offset) * tiling);
    texColor.a = alpha;
   
    return texColor;
}