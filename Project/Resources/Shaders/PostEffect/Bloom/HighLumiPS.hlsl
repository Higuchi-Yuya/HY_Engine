#include "HighLumiHeader.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex1 : register(t1); // 1番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 highLumiExtraction(float4 texColor)
{
    float grayScale = texColor.r * 0.299f + texColor.g * 0.587f + texColor.b * 0.114f;
    float extract = smoothstep(0.1f, 0.6f, grayScale);
    float power = 10.0f;
    
    float4 result = float4(texColor.rgb * extract * power, 1);
    
    return result;
}

float4 main(VSOutput input) : SV_TARGET
{
    PSOutput output;
    float4 texColor0 = tex.Sample(smp, input.uv);
    float4 texcolor1 = tex1.Sample(smp, input.uv);
    
    if (texcolor1.g >= 1.0f && texcolor1.b >= 1.0f)
    {
        float4 highLumiColor = highLumiExtraction(texColor0);
        
        return highLumiColor;

    }
    else
    {
        float4 black = { 0.0f, 0.0f, 0.0f, 1.0f };
        return black;

    }
    
   
    
    //float4 middleColor = highLumiColor + texcolor1;
    
    //float4 result = highLumiExtraction(middleColor);
    
    //output.target0 = result;
    
    //return result;
}