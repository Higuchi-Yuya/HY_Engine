#include "ObjLightAttenHeader.hlsli"

Texture2D<float4> tex : register(t0);  // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> dissolveTex : register(t1); // 1�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

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
	// �e�N�X�`���}�b�s���O
	
    float2 tilling = { 1, 1 };
    float2 offset = { 0, 0 };
    float4 texcolor = tex.Sample(smp, input.uv * tilling + offset);
	
	// ����x
	const float shininess = 32.0f;

	// ���_���王�_�ւ̕����x�N�g��
	float3 eyedir = normalize(cameraPos - input.worldpos.xyz);

	// �����ˌ�
    float3 ambientIndensity = m_ambient;
    float3 ambient = ambientIndensity * ambientColor;

	// �V�F�[�f�B���O�ɂ��F
    float4 shadecolor = { 0, 0, 0, 0.01f };

    if (bloomActive)
    {
        float4 result = { 0.0f, 1.0f, 1.0f, 1.0f };
        return result;
    }
	
	// ���s����
	for (int i = 0; i < DIRLIGHT_NUM; i++) {
		if (dirLights[i].active) {
			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
			float dotlightnormal = dot(dirLights[i].lightv, input.normal);

			float lightIntensity = smoothstep(0, 0.01, dotlightnormal);

			// ���ˌ��x�N�g��
			float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * input.normal);
			
			// �������C�g
            float rimDot = 1 - dot(eyedir, input.normal);
            float rimIntensity = smoothstep(0.716 - 0.01, 0.716 + 0.01, rimDot);
            float3 rimColor = { 1, 1, 1 };
            float3 rim = rimIntensity * rimColor;
			
			// �����ˌ�
            //float3 ambientIndensity = m_ambient;
            //float3 ambient = ambientIndensity * ambientColor;
            float3 ambient = m_ambient;
			
			// �g�U���ˌ�
			//float3 diffuseIndesity = lightIntensity * m_diffuse;
			//float3 diffuse = diffuseIndesity * diffuseColor;
            float3 diffuse = dotlightnormal * m_diffuse;
			
			// ���ʔ��ˌ�
			//float3 specularIdensity = smoothstep(0.005, 0.01, pow(saturate(dot(reflect, eyedir)), shininess) * m_specular);
			//float3 specular = specularIdensity * specularColor;
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;
			
			
			// �S�ĉ��Z����
            //float3 sColor = (1 - diffuseIndesity) * ambient;
            //sColor += diffuse;
            //sColor = (1 - specularIdensity) * sColor + specular;
            //sColor = (1 - rimIntensity) * sColor + rim;
			
            float3 sColor = (ambient + diffuse + specular);
			
			shadecolor.rgb += sColor * dirLights[i].lightcolor;
		}
	}

	// �_����
	for (i = 0; i < POINTLIGHT_NUM; i++) {
		if (pointLights[i].active) {
			// ���C�g�ւ̕����x�N�g��
			float3 lightv = pointLights[i].lightpos - input.worldpos.xyz;
			// �x�N�g���̒���
			float distance = length(lightv);
			// ���K�����A�P�ʃx�N�g���ɂ���
			lightv = normalize(lightv);

			// ���������W��
			//float atten = 1.0f / (pointLights[i].lightatten.x + pointLights[i].lightatten.y * d + pointLights[i].lightatten.z * d * d);

            float atten = pow(saturate(-distance / pointLights[i].lightRadius + 1.0f), pointLights[i].lightDecay);
			
            //float atten = attenuate_cusp(pointLights[i].lightDistance, pointLights[i].lightRadius, pointLights[i].lightIndensity, pointLights[i].lightDecay);
			
			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
			float3 dotlightnormal = dot(lightv, input.normal);
			// ���ˌ��x�N�g��
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			// �����ˌ�
            float3 ambient = m_ambient;
			// �g�U���ˌ�
			float3 diffuse = dotlightnormal * m_diffuse;
			// ���ʔ��ˌ�
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

            float3 sColor = (ambient + diffuse + specular);
			
            float3 lightColor = pointLights[i].lightcolor * pointLights[i].lightIndensity;
			
			// �S�ĉ��Z����
            shadecolor.rgb += sColor * lightColor * atten;
        }
	}

	// �X�|�b�g���C�g
	for (i = 0; i < SPOTLIGHT_NUM; i++) {
		if (spotLights[i].active) {
			// ���C�g�ւ̕����x�N�g��
			float3 lightv = spotLights[i].lightpos - input.worldpos.xyz;
			float distance = length(lightv);
			lightv = normalize(lightv);

			// ���������W��
            float atten = saturate(1.0f / 
			(spotLights[i].lightatten.x + 
			spotLights[i].lightatten.y * distance + 
			spotLights[i].lightatten.z * distance * distance));
			
			// �p�x����
            float2 lightVV = { lightv.x, lightv.z };
            float2 spotV = { spotLights[i].lightv.x, spotLights[i].lightv.z };
            float cos_ = dot(lightv, spotLights[i].lightv);
			// �����J�n�p�x����A�����I���p�x�ɂ����Č���
			// �����J�n�p�x�̓�����1�{ �����I���p�x�̊O����0�{�̋P�x
			float angleatten = smoothstep(spotLights[i].lightfactoranglecos.y, spotLights[i].lightfactoranglecos.x, cos_);
            float angleXZAtten = smoothstep(cos(DegreeToRadian(100)), cos(DegreeToRadian(10)), cos_);
			// �p�x��������Z
			atten *= angleatten;
            //atten = max(atten, 0.2f);

			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
			float3 dotlightnormal = dot(lightv, input.normal);
			// ���ˌ��x�N�g��
			float3 reflect = normalize(-lightv + 2 * dotlightnormal * input.normal);
			// �����ˌ�
            float3 ambient = m_ambient;
			// �g�U���ˌ�
			float3 diffuse = dotlightnormal * m_diffuse;
			// ���ʔ��ˌ�
			float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * m_specular;

			// �S�ĉ��Z����
            shadecolor.rgb += atten * (ambient + diffuse + specular) * spotLights[i].lightcolor;
            shadecolor.a += angleXZAtten * 0.8f;

        }
	}

	// �ۉe
	for (i = 0; i < CIRCLESHADOW_NUM; i++) {
		if (circleShadows[i].active) {
			// �I�u�W�F�N�g�\�ʂ���L���X�^�[�ւ̃x�N�g��
			float3 casterv = circleShadows[i].casterPos - input.worldpos.xyz;
			// ���������ł̋���
			float d = dot(casterv, circleShadows[i].dir);

			// ���������W��
			float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y * d + circleShadows[i].atten.z * d * d));
			// �������}�C�i�X�Ȃ�0�ɂ���
			atten *= step(0, d);

			// ���C�g�̍��W
			float3 lightpos = circleShadows[i].casterPos + circleShadows[i].dir * circleShadows[i].distanceCasterLight;
			//  �I�u�W�F�N�g�\�ʂ��烉�C�g�ւ̃x�N�g���i�P�ʃx�N�g���j
			float3 lightv = normalize(lightpos - input.worldpos.xyz);
			// �p�x����
			float cos = dot(lightv, circleShadows[i].dir);
			// �����J�n�p�x����A�����I���p�x�ɂ����Č���
			// �����J�n�p�x�̓�����1�{ �����I���p�x�̊O����0�{�̋P�x
			float angleatten = smoothstep(circleShadows[i].factorAngleCos.y, circleShadows[i].factorAngleCos.x, cos);
			// �p�x��������Z
			atten *= angleatten;

			// �S�Č��Z����
			shadecolor.rgb -= atten;
		}
	}

	// �����f�B�]���u���A�N�e�B�u�Ȃ�f�B�]���u�̏���������
    if (isActiveDissolve == true)
    {
		// �I�u�W�F�N�g�F
        float4 outputColor = color * shadecolor * texcolor;
		
		// �f�B�]���u�̃e�N�X�`��
        float4 dissolveTexColor = dissolveTex.Sample(smp, input.uv * tilling + offset);
		
		// �}�X�N��R�`�����l�����O�D�T�𒴂���ƕ`��j��
        float idensity = smoothstep(dissolveSmoothMin, 0.00, dissolveTexColor.r - dissolveTime);
        clip(idensity - dissolveTime);
		
        return outputColor + dissolveColor * dissolvePower * (1 - idensity);
    }
	
	// �����t�H�O���A�N�e�B�u�Ȃ�t�H�O�̏���������
    if (isActiveFog == true)
    {

		// �����̌v�Z
        float dis = distance(input.worldpos.xyz, cameraPos);

		// �t�H�O�̋����ɂ���Ă̌������v�Z
		//float rate = smoothstep(nearFog, farFog, dis);
        float f = (farFog - dis) / (farFog - nearFog);
        f = clamp(f, 0.0f, 1.0f);
		
		// �t�H�O�̐F�Ƌ��߂��t�H�O�̋������|���čŏI�I�ȃt�H�O�̃J���[���Z�o
		//float4 specialFogColor = rate * fogColor;

		// �I�u�W�F�N�g�F
        float4 outputColor = color * shadecolor * texcolor;

		// �t�H�O��S�̓I�ȃJ���[�����ɉ��Z
        return outputColor * f + fogColor * (1.0f - f);
    }

	// �e�N�X�`���̐F�͎g�킸�F�𐔒l�w��
	return color * shadecolor * texcolor;

}