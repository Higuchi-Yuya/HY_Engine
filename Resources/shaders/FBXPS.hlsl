#include "FBX.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

PSOutput main(GSOutput input)// : SV_TARGET
{
    PSOutput output;
	// テクスチャマッピング
	float4 texcolor = tex.Sample(smp, input.uv);
	// Lambert反射
	float3 light = normalize(float3(1,-1,1)); // 右下奥　向きのライト
	float diffuse = saturate(dot(-light, input.normal));
	float brightness = diffuse + 0.3f;
	//float brightness = 1;
	float4 shadecolor = float4(brightness, brightness, brightness, color.a);
	// 陰影とテクスチャの色を合成
    output.target0 = shadecolor * texcolor;
    output.target1 = float4(1 - (shadecolor * texcolor).rgb, 1);
    return output;
}