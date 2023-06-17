#include"FBX.hlsli"
float rand(float3 co)
{
    return frac(sin(dot(co.xyz, float3(12.9898, 78.233, 53.539))) * 43758.5453);
}

float3 rotate(float3 p, float3 rotation)
{
    //rotationがゼロ行列だと、Geometry shaderが表示されないので注意
    float3 a = normalize(rotation);
    float angle = length(rotation);
    //rotationがゼロ行列のときの対応
    if (abs(angle) < 0.001) return p;
    float s = sin(angle);
    float c = cos(angle);
    float r = 1.0 - c;
    float3x3 m = float3x3(
        a.x * a.x * r + c,
        a.y * a.x * r + a.z * s,
        a.z * a.x * r - a.y * s,
        a.x * a.y * r - a.z * s,
        a.y * a.y * r + c,
        a.z * a.y * r + a.x * s,
        a.x * a.z * r + a.y * s,
        a.y * a.z * r - a.x * s,
        a.z * a.z * r + c
        );

    return mul(m, p);
}


[maxvertexcount(3)]
void main(triangle VSOutput IN[3], inout TriangleStream<GSOutput> triStream)
{

    GSOutput o;

    float3 center = (IN[0].svpos.xyz + IN[1].svpos.xyz + IN[2].svpos.xyz) / 3;
    float3 r3 = rand(center);
    float3 up = float3(0, 1, 0);

    // 外積つかって、法線ベクトルの計算
    float3 vecA = IN[1].svpos.xyz - IN[0].svpos.xyz;
    float3 vecB = IN[2].svpos.xyz - IN[0].svpos.xyz;
    float3 normal = normalize(cross(vecA, vecB));

    [unroll]
    for (int i = 0; i < 3; i++)
    {
        VSOutput v = IN[i];

        // centerを起点に三角メッシュの大きさが変化
        v.svpos.xyz = center + (v.svpos.xyz - center) * (_ScaleFactor);

        // centerを起点に、頂点が回転
        v.svpos.xyz = center + rotate(v.svpos.xyz - center, r3 * (_Destruction * _RotationFactor));

        // 法線方向に弾け飛ぶ
        v.svpos.xyz += normal * (_Destruction * _PositionFactor) * r3;

        o.svpos = v.svpos;
        o.uv = v.uv;
        o.normal = v.normal;
        triStream.Append(o);
    }

    triStream.RestartStrip();
}