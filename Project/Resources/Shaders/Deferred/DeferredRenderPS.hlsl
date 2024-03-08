#include "DeferredRenderHeader.hlsli"

Texture2D<float4> colorMap : register(t0);  // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> worldPosMap : register(t1); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> cameraPosMap : register(t2); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> normalMap : register(t3); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> ambientMap : register(t4); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> diffuseMap : register(t5); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> specularMap : register(t6); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0);      // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutput input) : SV_TARGET
{	
	// �e�N�X�`���J���[
    float4 texcolor = colorMap.Sample(smp, input.uv);
    float4 worldPos = worldPosMap.Sample(smp, input.uv);
    float4 cameraPos = cameraPosMap.Sample(smp, input.uv);
    float4 normal = normalMap.Sample(smp, input.uv);
    float4 ambientValue = ambientMap.Sample(smp, input.uv);
    float4 diffuseValue = diffuseMap.Sample(smp, input.uv);
    float4 specularValue = specularMap.Sample(smp, input.uv);
	
	// ���ʓI�ȃJ���[
    float4 resultColor;
	
	// ����x
    const float shininess = 32.0f;

	// ���_���王�_�ւ̕����x�N�g��
    float3 eyedir = normalize(cameraPos.xyz - worldPos.xyz);
    
    // �V�F�[�f�B���O�ɂ��F
    float4 shadecolor = { 0, 0, 0, 1 };
    
    // ���s����
    for (int i = 0; i < DIRLIGHT_NUM; i++)
    {
        if (dirLights[i].active)
        {
			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float dotlightnormal = dot(dirLights[i].lightv, normal.xyz);

            float lightIntensity = smoothstep(0, 0.01, dotlightnormal);

			// ���ˌ��x�N�g��
            float3 reflect = normalize(-dirLights[i].lightv + 2 * dotlightnormal * normal.xyz);
			
			// �������C�g
            float rimDot = 1 - dot(eyedir, normal.xyz);
            float rimIntensity = smoothstep(0.716 - 0.01, 0.716 + 0.01, rimDot);
            float3 rimColor = { 1, 1, 1 };
            float3 rim = rimIntensity * rimColor;
			
			// �����ˌ�
            float3 ambient = ambientValue.xyz;
			
			// �g�U���ˌ�
            float3 diffuse = dotlightnormal * diffuseValue.xyz;
			
			// ���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * specularValue.xyz;
			
			
			// �S�ĉ��Z����		
            float3 sColor = (ambient + diffuse + specular);
			
            shadecolor.rgb += sColor * dirLights[i].lightcolor;
        }
    }

	// �_����
    for (i = 0; i < POINTLIGHT_NUM; i++)
    {
        if (pointLights[i].active)
        {
			// ���C�g�ւ̕����x�N�g��
            float3 lightv = pointLights[i].lightpos - worldPos.xyz;
			// �x�N�g���̒���
            float distance = length(lightv);
			// ���K�����A�P�ʃx�N�g���ɂ���
            lightv = normalize(lightv);

			// ���������W��
            float atten = pow(saturate(-distance / pointLights[i].lightRadius + 1.0f), pointLights[i].lightDecay);
			
			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = dot(lightv, normal.xyz);
			// ���ˌ��x�N�g��
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * normal.xyz);
			// �����ˌ�
            float3 ambient = ambientValue.xyz;
			// �g�U���ˌ�
            float3 diffuse = dotlightnormal * diffuseValue.xyz;
			// ���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * specularValue.xyz;

            float3 sColor = (ambient + diffuse + specular);
			
            float3 lightColor = pointLights[i].lightcolor * pointLights[i].lightIndensity;
			
			// �S�ĉ��Z����
            shadecolor.rgb += sColor * lightColor * atten;
        }
    }

	// �X�|�b�g���C�g
    for (i = 0; i < SPOTLIGHT_NUM; i++)
    {
        if (spotLights[i].active)
        {
			// ���C�g�ւ̕����x�N�g��
            float3 lightv = spotLights[i].lightpos - worldPos.xyz;
            float distance = length(lightv);
            lightv = normalize(lightv);

			// ���������W��
            float atten = saturate(1.0f /
			(spotLights[i].lightatten.x +
			spotLights[i].lightatten.y * distance +
			spotLights[i].lightatten.z * distance * distance));
			
			// �p�x����
            float cos = dot(lightv, spotLights[i].lightv);
			// �����J�n�p�x����A�����I���p�x�ɂ����Č���
			// �����J�n�p�x�̓�����1�{ �����I���p�x�̊O����0�{�̋P�x
            float angleatten = smoothstep(spotLights[i].lightfactoranglecos.y, spotLights[i].lightfactoranglecos.x, cos);
			// �p�x��������Z
            atten *= angleatten;

			// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
            float3 dotlightnormal = dot(lightv, normal.xyz);
			// ���ˌ��x�N�g��
            float3 reflect = normalize(-lightv + 2 * dotlightnormal * normal.xyz);
			// �����ˌ�
            float3 ambient = ambientValue.xyz;
			// �g�U���ˌ�
            float3 diffuse = saturate(dotlightnormal) * diffuseValue.xyz;
			// ���ʔ��ˌ�
            float3 specular = pow(saturate(dot(reflect, eyedir)), shininess) * specularValue.xyz;

            float3 sColor = (diffuse + specular);
			
            float3 lightColor = spotLights[i].lightcolor * spotLights[i].lightIndensity;
			
			// �S�ĉ��Z����
            shadecolor.rgb += atten * sColor * lightColor;
        }
    }

	// �ۉe
    for (i = 0; i < CIRCLESHADOW_NUM; i++)
    {
        if (circleShadows[i].active)
        {
			// �I�u�W�F�N�g�\�ʂ���L���X�^�[�ւ̃x�N�g��
            float3 casterv = circleShadows[i].casterPos - worldPos.xyz;
			// ���������ł̋���
            float d = dot(casterv, circleShadows[i].dir);

			// ���������W��
            float atten = saturate(1.0f / (circleShadows[i].atten.x + circleShadows[i].atten.y * d + circleShadows[i].atten.z * d * d));
			// �������}�C�i�X�Ȃ�0�ɂ���
            atten *= step(0, d);

			// ���C�g�̍��W
            float3 lightpos = circleShadows[i].casterPos + circleShadows[i].dir * circleShadows[i].distanceCasterLight;
			//  �I�u�W�F�N�g�\�ʂ��烉�C�g�ւ̃x�N�g���i�P�ʃx�N�g���j
            float3 lightv = normalize(lightpos - worldPos.xyz);
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
    
    // �����t�H�O���A�N�e�B�u�Ȃ�t�H�O�̏���������
    if (isActiveFog == true)
    {

		// �����̌v�Z
        float dis = distance(worldPos.xyz, cameraPos.xyz);

		// �t�H�O�̋����ɂ���Ă̌������v�Z
		//float rate = smoothstep(nearFog, farFog, dis);
        float f = (farFog - dis) / (farFog - nearFog);
        f = clamp(f, 0.0f, 1.0f);
		
		// �t�H�O�̐F�Ƌ��߂��t�H�O�̋������|���čŏI�I�ȃt�H�O�̃J���[���Z�o
		//float4 specialFogColor = rate * fogColor;

		// �I�u�W�F�N�g�F
        float4 outputColor = shadecolor * texcolor;

		// �t�H�O��S�̓I�ȃJ���[�����ɉ��Z
        return outputColor * f + fogColor * (1.0f - f);
    }
    
	// �e�N�X�`���̐F�͎g�킸�F�𐔒l�w��
    return shadecolor * texcolor;

}