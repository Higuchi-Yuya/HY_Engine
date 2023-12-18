#include "ObjLightAttenHeader.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
Texture2D<float4> dissolveTex : register(t1); // 1番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float sqr(float x)
{
    return x * x;
}

float attenuate_no_cusp(float distance, float radius, float max_intensity, float falloff)
{
    float s = distance / radius;

    if (s >= 1.0)
        return 0.0;

    float s2 = sqr(s);

    return max_intensity * sqr(1 - s2) / (1 + falloff * s2);
}

float attenuate_cusp(float distance, float radius, float max_intensity, float falloff)
{
    float s = distance / radius;

    if (s >= 1.0)
        return 0.0;

    float s2 = sqr(s);

    return max_intensity * sqr(1 - s2) / (1 + falloff * s);
}

float DegreeToRadian(float degree)
{
    float PI = 3.141592f;
    float result = degree * (PI / 180);

    return result;
}

float4 main(VSOutput input) : SV_TARGET
{	
	// テクスチャマッピング
	
    float2 tilling = { 1, 1 };
    float2 offset = { 0, 0 };
    float4 texcolor = tex.Sample(smp, input.uv * tilling + offset);
	
	// 光沢度
	const float shininess = 32.0f;

	// 頂点から視点への方向ベクトル
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	// 環境反射光
    float3 ambientIndensity = m_ambient;
    float3 ambient = ambientIndensity * ambientColor;

	// シェーディングによる色
    float4 shadecolor = { 0, 0, 0, 0.01f };

    if (bloomActive)
    {
        float4 result = { 0.0f, 1.0f, 1.0f, 1.0f };
        return result;
    }
	
	// 平行光源
	for (int i = 0; i < DIRLIGHT_NUM; i++) {
		if (dirLights[i].active) {
			// ライトに向かうベクトルと法線の内積
			float dotlightnormal = dot(dirLights[i].lightv, input.normal);

			float lightIntensity = smoothstep(0, 0.01, dotlightnormal);

			// 反射光ベクトル
			float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal);
			
			// リムライト
            float rimDot = 1 - dot(eyedir, input.normal);
            float rimIntensity = smoothstep(0.716 - 0.01, 0.716 + 0.01, rimDot);
            float3 rimColor = { 1, 1, 1 };
            float3 rim = rimIntensity * rimColor;
			
			// 環境反射光
            //float3 ambientIndensity = m_ambient;
            //float3 ambient = ambientIndensity * ambientColor;
            float3 ambient = m_ambient;
			
			// 拡散反射光
			//float3 diffuseIndesity = lightIntensity * m_diffuse;
			//float3 diffuse = diffuseIndesity * diffuseColor;
            float3 diffuse = dotlightnormal * m_diffuse;
			
			// 鏡面反射光
			//float3 specularIdensity = smoothstep(0.005, 0.01, pow(saturate(dot(reflect, eyedir)), shininess) * m_specular);
			//float3 specular = specularIdensity * specularColor;
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
			
			
			// 全て加算する
            //float3 sColor = (1 - diffuseIndesity) * ambient;
            //sColor += diffuse;
            //sColor = (1 - specularIdensity) * sColor + specular;
            //sColor = (1 - rimIntensity) * sColor + rim;
			
            float3 sColor = (ambient + diffuse + specular);
			
			shadecolor.rgb += sColor * dirLights[i].lightcolor;
		}
	}

	// 点光源
	for (i = 0; i < POINTLIGHT_NUM; i++) {
		if (pointLights[i].active) {
			// ライトへの方向ベクトル
			float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;
			// ベクトルの長さ
			float distance = length(lightv);
			// 正規化し、単位ベクトルにする
			lightv = normalize(lightv);

			// 距離減衰係数
			//float atten = 1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d + pointLights[i].lightatten.z * d * d);

            float atten = pow(saturate(-distance / pointLights[i].lightRadius + 1.0f), pointLights[i].lightDecay);
			
            //float atten = attenuate_cusp(pointLights[i].lightDistance, pointLights[i].lightRadius, pointLights[i].lightIndensity, pointLights[i].lightDecay);
			
			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(lightv, input.normal);
			// 反射光ベクトル
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			// 環境反射光
            float3 ambient = m_ambient;
			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

            float3 sColor = (ambient + diffuse + specular);
			
            float3 lightColor = pointLights[i].lightcolor * pointLights[i].lightIndensity;
			
			// 全て加算する
            shadecolor.rgb += sColor * lightColor * atten;
        }
	}

	// スポットライト
	for (i = 0; i < SPOTLIGHT_NUM; i++) {
		if (spotLights[i].active) {
			// ライトへの方向ベクトル
			float3 lightv = spotLights[i].lightpos - input.worldpos.xyz;
			float distance = length(lightv);
			lightv = normalize(lightv);

			// 距離減衰係数
            float atten = saturate(1.0f / 
			(spotLights[i].lightatten.x + 
			spotLights[i].lightatten.y * distance + 
			spotLights[i].lightatten.z * distance * distance));
			
			// 角度減衰
            float2 lightVV = { lightv.x, lightv.z };
            float2 spotV = { spotLights[i].lightv.x, spotLights[i].lightv.z };
            float cos_ = dot(lightv, spotLights[i].lightv);
			// 減衰開始角度から、減衰終了角度にかけて減衰
			// 減衰開始角度の内側は1倍 減衰終了角度の外側は0倍の輝度
			float angleatten = smoothstep(spotLights[i].lightfactoranglecos.y, spotLights[i].lightfactoranglecos.x, cos_);
            float angleXZAtten = smoothstep(cos(DegreeToRadian(100)), cos(DegreeToRadian(10)), cos_);
			// 角度減衰を乗算
			atten *= angleatten;
            //atten = max(atten, 0.2f);

			// ライトに向かうベクトルと法線の内積
			float3 dotlightnormal = dot(lightv, input.normal);
			// 反射光ベクトル
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			// 環境反射光
            float3 ambient = m_ambient;
			// 拡散反射光
			float3 diffuse = dotlightnormal * m_diffuse;
			// 鏡面反射光
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			// 全て加算する
            shadecolor.rgb += atten * (ambient + diffuse + specular) * spotLights[i].lightcolor;
            shadecolor.a += angleXZAtten * 0.8f;

        }
	}

	// 丸影
	for (i = 0; i < CIRCLESHADOW_NUM; i++) {
		if (circleShadows[i].active) {
			// オブジェクト表面からキャスターへのベクトル
			float3 casterv = circleShadows[i].casterPos - input.worldpos.xyz;
			// 光線方向での距離
			float d = dot(casterv, circleShadows[i].dir);

			// 距離減衰係数
			float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y * d + circleShadows[i].atten.z * d * d));
			// 距離がマイナスなら0にする
			atten *= step(0, d);

			// ライトの座標
			float3 lightpos = circleShadows[i].casterPos + circleShadows[i].dir * circleShadows[i].distanceCasterLight;
			//  オブジェクト表面からライトへのベクトル（単位ベクトル）
			float3 lightv = normalize(lightpos - input.worldpos.xyz);
			// 角度減衰
			float cos = dot(lightv, circleShadows[i].dir);
			// 減衰開始角度から、減衰終了角度にかけて減衰
			// 減衰開始角度の内側は1倍 減衰終了角度の外側は0倍の輝度
			float angleatten = smoothstep(circleShadows[i].factorAngleCos.y, circleShadows[i].factorAngleCos.x, cos);
			// 角度減衰を乗算
			atten *= angleatten;

			// 全て減算する
			shadecolor.rgb -= atten;
		}
	}

	// もしディゾルブがアクティブならディゾルブの処理をする
    if (isActiveDissolve == true)
    {
		// オブジェクト色
        float4 outputColor = color * shadecolor * texcolor;
		
		// ディゾルブのテクスチャ
        float4 dissolveTexColor = dissolveTex.Sample(smp, input.uv * tilling + offset);
		
		// マスクのRチャンネルが０．５を超えると描画破棄
        float idensity = smoothstep(dissolveSmoothMin, 0.00, dissolveTexColor.r - dissolveTime);
        clip(idensity - dissolveTime);
		
        return outputColor + dissolveColor * dissolvePower * (1 - idensity);
    }
	
	// もしフォグがアクティブならフォグの処理をする
    if (isActiveFog == true)
    {

		// 距離の計算
        float dis = distance(input.worldpos.xyz, cameraPos);

		// フォグの距離によっての減衰を計算
		//float rate = smoothstep(nearFog, farFog, dis);
        float f = (farFog - dis) / (farFog - nearFog);
        f = clamp(f, 0.0f, 1.0f);
		
		// フォグの色と求めたフォグの距離を掛けて最終的なフォグのカラーを算出
		//float4 specialFogColor = rate * fogColor;

		// オブジェクト色
        float4 outputColor = color * shadecolor * texcolor;

		// フォグを全体的なカラー処理に加算
        return outputColor * f + fogColor * (1.0f - f);
    }

	// テクスチャの色は使わず色を数値指定
	return color * shadecolor * texcolor;

}