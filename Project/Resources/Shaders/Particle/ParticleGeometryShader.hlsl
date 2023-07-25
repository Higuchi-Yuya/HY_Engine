#include "ParticleShaderHeader.hlsli"

// 四角形の頂点数
static const uint vnum = 4;

// センターからのオフセット
static const float4 offset_array[vnum] =
{
	float4(-0.5f,-0.5f,0,0), // 左下
	float4(-0.5f,+0.5f,0,0), // 左上
	float4(+0.5f,-0.5f,0,0), // 右下
	float4(+0.5f,+0.5f,0,0), // 右上
};

static const float2 uv_array[vnum] =
{
	float2(0,1),
	float2(0,0),
	float2(1,1),
	float2(1,0)
};

// 三角形の入力から、点を三つ出力するサンプル
[maxvertexcount(vnum)]
void main(
	point VSOutput input[1] : SV_POSITION,
	// 点ストリーム
	inout TriangleStream< GSOutput > output
)
{
    GSOutput element = (GSOutput)0; // 出力頂点データ
	for (uint i = 0; i < vnum; i++) {
		// 中心からのオフセットをビルボード回転（モデル座標）
		float4 offset;
		//offset = mul(matBillboard, offset_array[i]);
		// 中心からオフセットをスケーリング
		offset = offset_array[i] * input[0].scale;

		// 中心からのオフセットをビルボード回転（モデル座標）
		offset = mul(matBillboard, offset);

		// ワールド座標ベースで、ずらす
		element.svpos = input[0].pos + offset;// 頂点座標をコピー
		
		// ビュー、射影変換
		element.svpos = mul(mat, element.svpos);
		element.uv = uv_array[i];
        element.color = input[0].color;
		output.Append(element);
	}

}