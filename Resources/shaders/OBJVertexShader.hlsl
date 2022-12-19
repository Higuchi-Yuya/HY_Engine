#include "OBJShaderHeader.hlsli"

VSOutput main(float4 pos : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
	// 法線にワールド行列によるスケーリング・回転を適用
	// ※スケーリングが一様な場合のみ正しい
	float4 wnormal = normalize(mul(world, float4(normal, 0)));
	float4 wpos = mul(world, pos);

	//// 右、下、奥の方向を向いたライト
	//float3 lightv = float3(1, 0, 0);
	//lightv = normalize(lightv);
	// ライトの色（白）
	//float3 lightcolor = float3(1, 1, 1);

	VSOutput output; // ピクセルシェーダーに渡す値
	output.svpos = mul(mul(mul(projection, view), world), pos);

	// 環境反射光 
	float3 ambient = m_ambient;
	// 拡散反射光
	float3 diffuse = dot(lightv, wnormal.xyz) * m_diffuse;
	//// 視点座標
	//const float3 eye = float3(0, 0, -20);
	// 光沢度
	const float shininess = 4.0f;
	// 頂点から視点への方向ベクトル
	float3 eyedir = normalize(cameraPos - wpos.xyz);
	// 反射光ベクトル
	float3 reflect = normalize(-lightv + 2 * dot(lightv, wnormal.xyz) * wnormal.xyz);
	// 鏡面反射光
	float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

	output.worldpos = wpos;
	// 全て加算する
	output.color.rgb = (ambient + diffuse + specular) * lightcolor;
	output.color.a = m_alpha;
	//output.normal = wnormal.xyz;
	output.uv = uv;

	return output;
}