#include "DeferredObjHeader.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	// 法線にワールド行列によるスケーリング・回転を適用
	// ※スケーリングが一様な場合のみ正しい
    float4 wpos = mul(world, pos);
	float4 wnormal = normalize(mul(world, float4(normal, 0)));

	VSOutput output; // ピクセルシェーダーに渡す値
	output.worldpos = wpos;
	output.svpos = mul(mul(mul(projection, view), world), pos);
	output.normal = wnormal.xyz;
	output.uv = uv;

	return output;
}