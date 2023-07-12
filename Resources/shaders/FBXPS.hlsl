#include "FBX.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float4 main(GSOutput input) : SV_TARGET
{
	// テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv);
	// Lambert反射
    float3 light = normalize(float3(1, -1, 1)); // 右下奥　向きのライト
    float diffuse = saturate(dot(-light, input.normal));
    float brightness = diffuse + 0.3f;
    float4 shadecolor = float4(brightness, brightness, brightness, color.a);
	
    return shadecolor * texcolor;
}