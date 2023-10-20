#include "SilhouetteHeader.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{	
	// テクスチャマッピング
	
    float2 tilling = { 1, 1 };
    float2 offset = { 0, 0 };
    float4 texcolor = tex.Sample(smp, input.uv * tilling + offset);

    float4 color = { 0.1f, 0.1f, 0.1f, 1 };
    
    return color;

}