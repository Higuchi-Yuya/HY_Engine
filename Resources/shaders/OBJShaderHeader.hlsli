cbuffer WorldTransform : register(b0)
{
	float4 color; // 色（RGBA）
	matrix world; // ３Ｄ変換行列
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

// 平行光源の数
static const int DIRLIGHT_NUM = 3;

struct DirLight
{
	float3 lightv;    // ライトへの方向の単位ベクトル
	float3 lightcolor;    // ライトの色(RGB)
	uint active;
};

// 点光源の数
static const int POINTLIGHT_NUM = 3;

struct PointLight
{
	float3 lightpos;    // ライト座標
	float3 lightcolor;  // ライトの色(RGB)
	float3 lightatten;	// ライト距離減衰係数
	uint active;
};

// スポットライトの数
static const int SPOTLIGHT_NUM = 3;

struct SpotLight
{
	float3 lightv;		// ライトの光線方向の逆ベクトル（単位ベクトル）
	float3 lightpos;    // ライト座標
	float3 lightcolor;  // ライトの色(RGB)
	float3 lightatten;	// ライト距離減衰係数
	float2 lightfactoranglecos; // ライト減衰角度のコサイン
	uint active;
};

// 丸影の数
static const int CIRCLESHADOW_NUM = 3;

struct CircleShadow
{
	float3 dir;		// 投影方向の逆ベクトル（単位ベクトル）
	float3 casterPos;    // キャスター座標
	float  distanceCasterLight;	// キャスターとライトの距離
	float3 atten;	// 距離減衰係数
	float2 factorAngleCos; // 減衰角度のコサイン
	uint active;
};

// ライトグループ
cbuffer LightGroup : register(b3)
{
	float3 ambientColor;
	float3 diffuseColor;// ディフューズカラー
	float3 specularColor;// スペキュラーカラー
	
	DirLight dirLights[DIRLIGHT_NUM];
	PointLight pointLights[POINTLIGHT_NUM];
	SpotLight spotLights[SPOTLIGHT_NUM];
	CircleShadow circleShadows[CIRCLESHADOW_NUM];
}

// フォグ
cbuffer Fog : register(b4)
{
	float4 fogColor;
	bool isActiveFog;
	float nearFog;
	float farFog;
}

// ディゾルブ
cbuffer Dissolve : register(b5)
{
    float4 dissolveColor;
    bool isActiveDissolve;
    float dissolvePower;
    float dissolveTime;
    float dissolveSmoothMin;
}

// 頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos : SV_POSITION; // システム用頂点座標
	float4 worldpos : POSITION;	//ワールド座標
	float3 normal :NORMAL; // 法線ベクトル
	float2 uv  :TEXCOORD; // uv値
};