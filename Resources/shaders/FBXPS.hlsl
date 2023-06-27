#include "FBX.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

float Gaussian(float2 drawUV, float2 pickUV, float sigma)
{
    float d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

PSOutput main(GSOutput input)// : SV_TARGET
{
    PSOutput output;
	// テクスチャマッピング
    float4 texcolor = tex.Sample(smp, input.uv);
	// Lambert反射
    float3 light = normalize(float3(1, -1, 1)); // 右下奥　向きのライト
    float diffuse = saturate(dot(-light, input.normal));
    float brightness = diffuse + 0.3f;
	//float brightness = 1;
    float4 shadecolor = float4(brightness, brightness, brightness, color.a);
	
    
    float totalWeight = 0, _Sigma = 0.005, _StepWidth = 0.001;
    float4 col = float4(0, 0, 0, 0);

    for (float py = -_Sigma * 2; py <= _Sigma * 2; py += _StepWidth)
    {
        for (float px = -_Sigma * 2; px <= _Sigma * 2; px += _StepWidth)
        {
            float2 pickUV = input.uv + float2(px, py);
            float weight = Gaussian(input.uv, pickUV, _Sigma);
            col.rgb += tex.Sample(smp, pickUV).rgb * weight;
            totalWeight += weight;
        }
    }
    col.rgb = col.rgb / totalWeight;
    col.a = 1;
    

	
	// 陰影とテクスチャの色を合成
    output.target0 = float4(1 - (shadecolor * texcolor).rgb, 1);
    output.target1 = float4(col.rgb, 1);
    return output;
}