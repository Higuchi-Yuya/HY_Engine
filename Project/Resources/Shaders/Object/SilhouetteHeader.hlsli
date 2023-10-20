cbuffer WorldTransform : register(b0)
{
    float4 color; // 色（RGBA）
    matrix world; // ３Ｄ変換行列
};

cbuffer ViewProjection : register(b1)
{
    matrix view; // ビュー変換行列
    matrix projection; // プロジェクション変換行列
    float3 cameraPos; // カメラ座標（ワールド座標
}

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos : SV_POSITION; // システム用頂点座標
	float4 worldpos : POSITION;	//ワールド座標
	float3 normal :NORMAL; // 法線ベクトル
	float2 uv  :TEXCOORD; // uv値
};