#include "DeferredRenderHeader.hlsli"

Texture2D<float4> colorMap : register(t0);  // 0番スロットに設定されたテクスチャ
Texture2D<float4> worldPosMap : register(t1); // 0番スロットに設定されたテクスチャ
Texture2D<float4> cameraPosMap : register(t2); // 0番スロットに設定されたテクスチャ
Texture2D<float4> normalMap : register(t3); // 0番スロットに設定されたテクスチャ
Texture2D<float4> ambientMap : register(t4); // 0番スロットに設定されたテクスチャ
Texture2D<float4> diffuseMap : register(t5); // 0番スロットに設定されたテクスチャ
Texture2D<float4> specularMap : register(t6); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{	
	// テクスチャカラー
    float4 texcolor = colorMap.Sample(smp, input.uv);
    float4 worldPos = worldPosMap.Sample(smp, input.uv);
    float4 cameraPos = cameraPosMap.Sample(smp, input.uv);
    float4 normal = normalMap.Sample(smp, input.uv);
    float4 ambientValue = ambientMap.Sample(smp, input.uv);
    float4 diffuseValue = diffuseMap.Sample(smp, input.uv);
    float4 specularValue = specularMap.Sample(smp, input.uv);
	
	// 結果的なカラー
    float4 resultColor;
	
	// 光沢度
    const float shininess = 32.0f;

	// 頂点から視点への方向ベクトル
    float3 eyedir = normalize(cameraPos.xyz - worldPos.xyz);
    
    // シェーディングによる色
    float4 shadecolor = { 0, 0, 0, 1 };
    
    // 平行光源
    for (int i = 0; i < DIRLIGHT_NUM; i++)
    {
        if (dirLights[i].active)
        {
			// ライトに向かうベクトルと法線の内積
            float dotlightnormal = dot(dirLights[i].lightv, normal.xyz);

            float lightIntensity = smoothstep(0, 0.01, dotlightnormal);

			// 反射光ベクトル
            float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * normal.xyz);
			
			// リムライト
            float rimDot = 1 - dot(eyedir, normal.xyz);
            float rimIntensity = smoothstep(0.716 - 0.01, 0.716 + 0.01, rimDot);
            float3 rimColor = { 1, 1, 1 };
            float3 rim = rimIntensity * rimColor;
			
			// 環境反射光
            float3 ambient = ambientValue.xyz;
			
			// 拡散反射光
            float3 diffuse = dotlightnormal * diffuseValue.xyz;
			
			// 鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * specularValue.xyz;
			
			
			// 全て加算する		
            float3 sColor = (ambient + diffuse + specular);
			
            shadecolor.rgb += sColor * dirLights[i].lightcolor;
        }
    }

	// 点光源
    for (i = 0; i < POINTLIGHT_NUM; i++)
    {
        if (pointLights[i].active)
        {
			// ライトへの方向ベクトル
            float3 lightv = pointLights[i].lightpos - worldPos.xyz;
			// ベクトルの長さ
            float distance = length(lightv);
			// 正規化し、単位ベクトルにする
            lightv = normalize(lightv);

			// 距離減衰係数
            float atten = pow(saturate(-distance / pointLights[i].lightRadius + 1.0f), pointLights[i].lightDecay);
			
			// ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightv, normal.xyz);
			// 反射光ベクトル
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * normal.xyz);
			// 環境反射光
            float3 ambient = ambientValue.xyz;
			// 拡散反射光
            float3 diffuse = dotlightnormal * diffuseValue.xyz;
			// 鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * specularValue.xyz;

            float3 sColor = (ambient + diffuse + specular);
			
            float3 lightColor = pointLights[i].lightcolor * pointLights[i].lightIndensity;
			
			// 全て加算する
            shadecolor.rgb += sColor * lightColor * atten;
        }
    }

	// スポットライト
    for (i = 0; i < SPOTLIGHT_NUM; i++)
    {
        if (spotLights[i].active)
        {
			// ライトへの方向ベクトル
            float3 lightv = spotLights[i].lightpos - worldPos.xyz;
            float distance = length(lightv);
            lightv = normalize(lightv);

			// 距離減衰係数
            float atten = saturate(1.0f /
			(spotLights[i].lightatten.x +
			spotLights[i].lightatten.y * distance +
			spotLights[i].lightatten.z * distance * distance));
			
			// 角度減衰
            float cos = dot(lightv, spotLights[i].lightv);
			// 減衰開始角度から、減衰終了角度にかけて減衰
			// 減衰開始角度の内側は1倍 減衰終了角度の外側は0倍の輝度
            float angleatten = smoothstep(spotLights[i].lightfactoranglecos.y, spotLights[i].lightfactoranglecos.x, cos);
			// 角度減衰を乗算
            atten *= angleatten;

			// ライトに向かうベクトルと法線の内積
            float3 dotlightnormal = dot(lightv, normal.xyz);
			// 反射光ベクトル
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * normal.xyz);
			// 環境反射光
            float3 ambient = ambientValue.xyz;
			// 拡散反射光
            float3 diffuse = saturate(dotlightnormal) * diffuseValue.xyz;
			// 鏡面反射光
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * specularValue.xyz;

            float3 sColor = (diffuse + specular);
			
            float3 lightColor = spotLights[i].lightcolor * spotLights[i].lightIndensity;
			
			// 全て加算する
            shadecolor.rgb += atten * sColor * lightColor;
        }
    }

	// 丸影
    for (i = 0; i < CIRCLESHADOW_NUM; i++)
    {
        if (circleShadows[i].active)
        {
			// オブジェクト表面からキャスターへのベクトル
            float3 casterv = circleShadows[i].casterPos - worldPos.xyz;
			// 光線方向での距離
            float d = dot(casterv, circleShadows[i].dir);

			// 距離減衰係数
            float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y * d + circleShadows[i].atten.z * d * d));
			// 距離がマイナスなら0にする
            atten *= step(0, d);

			// ライトの座標
            float3 lightpos = circleShadows[i].casterPos + circleShadows[i].dir * circleShadows[i].distanceCasterLight;
			//  オブジェクト表面からライトへのベクトル（単位ベクトル）
            float3 lightv = normalize(lightpos - worldPos.xyz);
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
    
    // もしフォグがアクティブならフォグの処理をする
    if (isActiveFog == true)
    {

		// 距離の計算
        float dis = distance(worldPos.xyz, cameraPos.xyz);

		// フォグの距離によっての減衰を計算
		//float rate = smoothstep(nearFog, farFog, dis);
        float f = (farFog - dis) / (farFog - nearFog);
        f = clamp(f, 0.0f, 1.0f);
		
		// フォグの色と求めたフォグの距離を掛けて最終的なフォグのカラーを算出
		//float4 specialFogColor = rate * fogColor;

		// オブジェクト色
        float4 outputColor = shadecolor * texcolor;

		// フォグを全体的なカラー処理に加算
        return outputColor * f + fogColor * (1.0f - f);
    }
    
	// テクスチャの色は使わず色を数値指定
    return shadecolor * texcolor;

}