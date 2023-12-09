cbuffer cbuff0 : register(b0)
{
	float2 centerPos; // 中心座標
	bool isActive;    // 波紋をオンにするかどうか
    float alpha;      // 輝度：0～1
    float waveSpan;   // 波の数 多いほど細かい：0～100
    float waveFrame;  // 中心座標から広がっていく速度：0～1
    float waveScale;  // 波の間隔：0～0.1f 0.1以上だと大きくなりすぎる
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