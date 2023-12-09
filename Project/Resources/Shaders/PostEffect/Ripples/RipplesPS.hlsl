#include "RipplesHeader.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー


float4 main(VSOutput input) : SV_TARGET
{   
    if (isActive == true)
    {
        input.uv.x -= centerPos.x;
        input.uv.y -= centerPos.y;
    
        float dist = sqrt(input.uv.x * input.uv.x + input.uv.y * input.uv.y);
        float ang = atan2(input.uv.y, input.uv.x);
 
        input.uv.x = centerPos.x + cos(ang) * (dist * (1.0 + sin(dist * waveSpan + waveFrame * 31.4) * waveScale));
        input.uv.y = centerPos.y + sin(ang) * (dist * (1.0 + sin(dist * waveSpan + waveFrame * 31.4) * waveScale));
    }
    
    float4 texColor = tex.Sample(smp, input.uv);
    texColor.a = alpha;
   
    return texColor;
}