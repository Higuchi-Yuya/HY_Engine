// 平行光源の数
static const int DIRLIGHT_NUM = 3;

struct DirLight
{
    float3 lightv; // ライトへの方向の単位ベクトル
    float3 lightcolor; // ライトの色(RGB)
    uint active;
};

// 点光源の数
static const int POINTLIGHT_NUM = 30;

struct PointLight
{
    float3 lightpos; // ライト座標
    float3 lightcolor; // ライトの色(RGB)
    float3 lightatten; // ライト距離減衰係数
    float lightIndensity;
    float lightRadius;
    float lightDecay;
    uint active;
};

// スポットライトの数
static const int SPOTLIGHT_NUM = 3;

struct SpotLight
{
    float3 lightv; // ライトの光線方向の逆ベクトル（単位ベクトル）
    float3 lightpos; // ライト座標
    float3 lightcolor; // ライトの色(RGB)
    float3 lightatten; // ライト距離減衰係数
    float2 lightfactoranglecos; // ライト減衰角度のコサイン
    float lightIndensity; // ライトの輝度
    uint active;
};

// 丸影の数
static const int CIRCLESHADOW_NUM = 3;

struct CircleShadow
{
    float3 dir; // 投影方向の逆ベクトル（単位ベクトル）
    float3 casterPos; // キャスター座標
    float distanceCasterLight; // キャスターとライトの距離
    float3 atten; // 距離減衰係数
    float2 factorAngleCos; // 減衰角度のコサイン
    uint active;
};

// ライトグループ
cbuffer LightGroup : register(b0)
{	
    DirLight dirLights[DIRLIGHT_NUM];
    PointLight pointLights[POINTLIGHT_NUM];
    SpotLight spotLights[SPOTLIGHT_NUM];
    CircleShadow circleShadows[CIRCLESHADOW_NUM];
}

// フォグ
cbuffer Fog : register(b1)
{
    float4 fogColor;
    bool isActiveFog;
    float nearFog;
    float farFog;
}

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
    float4 svpos : SV_POSITION; // システム用頂点座標
    float2 uv : TEXCOORD; // uv値
};