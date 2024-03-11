#include "PostDeferredHeader.hlsli"

Texture2D<float4> colorMap : register(t0);  // 0番スロットに設定されたテクスチャ
Texture2D<float4> worldPosMap : register(t1); // 0番スロットに設定されたテクスチャ
Texture2D<float4> cameraPosMap : register(t2); // 0番スロットに設定されたテクスチャ
Texture2D<float4> normalMap : register(t3); // 0番スロットに設定されたテクスチャ
Texture2D<float4> ambientMap : register(t4); // 0番スロットに設定されたテクスチャ
Texture2D<float4> diffuseMap : register(t5); // 0番スロットに設定されたテクスチャ
Texture2D<float4> specularMap : register(t6); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{	
	// テクスチャカラー
    float4 texcolor = colorMap.Sample(smp, input.uv);
    float4 worldPos = worldPosMap.Sample(smp, input.uv);
    float4 cameraPos = cameraPosMap.Sample(smp, input.uv);
    float4 normal = normalMap.Sample(smp, input.uv);
    float4 ambientValue = ambientMap.Sample(smp, input.uv);
    float4 diffuseValue = diffuseMap.Sample(smp, input.uv);
    float4 specularValue = specularMap.Sample(smp, input.uv);
    
    if (texNum == 1)
    {
        return worldPos;
    }
    else if (texNum == 2)
    {
        return cameraPos;
    }
    else if (texNum == 3)
    {
        return normal;
    }
    else if (texNum == 4)
    {
        return ambientValue;
    }
    else if (texNum == 5)
    {
        return diffuseValue;
    }
    else if (texNum == 6)
    {
        return specularValue;
    }
    return texcolor;
}