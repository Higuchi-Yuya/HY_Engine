cbuffer WorldTransform : register(b0)
{
	float4 color; // 色（RGBA）
	matrix world; // ３Ｄ変換行列
    uint bloomActive;
};

cbuffer ViewProjection : register(b1)
{
	matrix view;       // ビュー変換行列
	matrix projection; // プロジェクション変換行列
	float3 cameraPos;  // カメラ座標（ワールド座標
}

cbuffer cbuff1:register(b2)
{
	float3 m_ambient;//:packoffset(c0);// アンビエント係数
	float3 m_diffuse;//:packoffset(c1);// ディフューズ係数
	float3 m_specular;//:packoffset(c2);// スペキュラー係数
	float m_alpha;//: packoffset(c2.w);// アルファ
}

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos : SV_POSITION; // システム用頂点座標
	float4 worldpos : POSITION;	//ワールド座標
	float3 normal :NORMAL; // 法線ベクトル
	float2 uv  :TEXCOORD; // uv値
};

struct PSOutput
{
    float4 ColorMap : SV_Target0;
    float4 WorldPosMap : SV_Target1;
    float4 NomalMap : SV_Target2;
    float4 AmbientMap : SV_Target3;
    float4 DiffuseMap : SV_Target4;
    float4 SpecularMap : SV_Target5;
};