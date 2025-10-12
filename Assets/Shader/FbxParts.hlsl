#include "3DCommon.hlsli"

VS_OUT VS(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
    VS_OUT outData;

    outData.position = mul(position, g_matrixWVP);

    // �@���̕ό`
    normal.w = 0;
    outData.normal = mul(normal, g_matrixNormalTrans);

    
    float4 worldPosition = mul(position, g_matrixW);
    // �����x�N�g��
    outData.eye = normalize(g_cameraPosition - worldPosition);  // ���_�ւ̎���
    // UV���W
    outData.uv = uv;
    
    return outData;
}

float4 PS(VS_OUT inData) : SV_Target
{
    // ��������
    float4 lightDir = normalize(g_lightDir);
    
    // �@��
    inData.normal = normalize(inData.normal);
    
    // 
    float4 shade = saturate(dot(inData.normal, -lightDir));
    shade.a = 1;  // �����x�͑��삵�����Ȃ����߁A�����I�ɃA���t�@�l1
    
    
    float4 diffuse;
    if (g_hasTexture == true)
    {
        //diffuse = g_diffuseColor;
        // �e�N�X�`��
        diffuse = g_texture.Sample(g_sampler, inData.uv);

    }
    else
    {
        // �g�U���ː���
        diffuse = g_diffuseColor;
        //diffuse = float4(255, 1, 1, 0.5);
    }
    
    // ����
    //float4 ambient = g_ambientColor;
    float4 ambient = float4(1,1,1,1);
    
    // ���ʔ��ː��� (��������0��)
    float4 specuer = float4(0, 0, 0, 0);
    //g_speculerColor //float4(255, 0, 0, 0);
    if (g_speculerColor.a != 0)
    {
        // �����˃x�N�g��
        float4 r = reflect(lightDir, inData.normal);
        // ���ʔ��ː����v�Z
        specuer = pow(saturate(dot(r, inData.eye)), g_shuniness) * g_speculerColor;
    }
    
    //return float4(1, 0, 0, 1);
    //return diffuse * shade + diffuse * g_ambientColor + specuer;
    //float4 color = diffuse * shade + g_ambientColor + specuer;
    //color.a = 1;
    
    // �ŏI�I�ȐF
    float4 color = diffuse * shade + diffuse * ambient + specuer;
    return color;

}
