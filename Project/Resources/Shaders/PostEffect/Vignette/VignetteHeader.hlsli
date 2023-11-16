cbuffer cbuff0 : register(b0)
{
	float4 vignetteColor; // 色(RGBA)
	float vignetteRadius; // 広さ
    float vignetteFeather;
};

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos : SV_POSITION; // システム用頂点座標
	float2 uv  :TEXCOORD; // uv値
};

struct PSOutput
{
    float4 target0 : SV_TARGET0;
};