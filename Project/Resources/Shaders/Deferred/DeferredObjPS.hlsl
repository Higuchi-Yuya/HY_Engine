#include "DeferredObjHeader.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

PSOutput main(VSOutput input) : SV_TARGET
{	
	// テクスチャマッピング
    PSOutput output;
    float4 texColor = tex.Sample(smp, input.uv);
    float4 baseColor = color * texColor;
    
    output.ColorMap = baseColor;
    output.WorldPosMap = float4(input.worldpos.x, input.worldpos.y, input.worldpos.z, 1.0f);
    output.NomalMap = float4(input.normal.x, input.normal.y, input.normal.z, 1.0f);
    output.AmbientMap = float4(m_ambient, 1.0f);
    output.DiffuseMap = float4(m_diffuse, 1.0f);
    output.SpecularMap = float4(m_specular, 1.0f);
    
	// テクスチャの色は使わず色を数値指定
	return output;

}