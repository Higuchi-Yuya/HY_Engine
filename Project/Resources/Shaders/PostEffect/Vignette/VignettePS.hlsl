#include "VignetteHeader.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー


float4 main(VSOutput input) : SV_TARGET
{   
    float4 texColor = tex.Sample(smp, input.uv.xy);
    
    float2 newUV = input.uv.xy * 2 - 1;
    float circle = length(newUV);
    float mask = 1 - smoothstep(vignetteRadius, vignetteRadius + vignetteFeather, circle);
    float invertMask = 1 - mask;
    
    float3 displayCol = texColor.rgb * mask;
    float3 vignetteCol = (1 - texColor.rgb) * vignetteColor.rgb * invertMask;
    
    float4 result = float4(displayCol + vignetteCol, 1);
    
    return result;
}