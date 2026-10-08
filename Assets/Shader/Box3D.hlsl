#include "3DCommon.hlsli"

// 受け取るキャスターの最大数
#define MAX_CASTER_COUNT 128

struct Caster
{
    float4 pos;
    float radius;
    float3 caster_padding;
};

cbuffer ShadowParam : register(b1)
{
    Caster casters[MAX_CASTER_COUNT];
    int casterCount;
    float3 sp_padding;
}

struct BOX3D_VS_OUT
{
    float4 position : SV_POSITION; // 位置
    float4 normal : NORMAL0; // 法線
    float2 uv : TEXCOORD; // uv座標
    float4 eye : NORMAL1;
    float4 positionW : POSITION0;
};

BOX3D_VS_OUT VS(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
    BOX3D_VS_OUT outData;

    outData.position = mul(position, g_matrixWVP);

    // 法線の変形
    normal.w = 0;
    outData.normal = mul(normal, g_matrixNormalTrans);

    float4 worldPosition = mul(position, g_matrixW);
    outData.positionW = worldPosition;
    // 視線ベクトル
    outData.eye = normalize(g_cameraPosition - worldPosition);
    
    // UV座標
    outData.uv = uv;
    
    return outData;
}

float4 PS(BOX3D_VS_OUT inData) : SV_Target
{
    // 光源方向
    float4 lightDir = normalize(g_lightDir);
    
    // 法線
    inData.normal = normalize(inData.normal);
    
    float4 shade = saturate(dot(inData.normal, -lightDir));
    shade.a = 1;
    
    
    float4 diffuse;
    if (g_hasTexture == true)
    {
        diffuse = g_texture.Sample(g_sampler, inData.uv);
    }
    else
    {
        diffuse = g_diffuseColor;
    }
    
    // 環境光
    float4 ambient = float4(1, 1, 1, 1);
    
    // 鏡面反射成分
    float4 speculer = float4(0, 0, 0, 0);
    if (g_speculerColor.a != 0)
    {
        float4 r = reflect(lightDir, inData.normal);
        speculer = pow(saturate(dot(r, inData.eye)), g_shuniness) * g_speculerColor;
    }
    float shadowAlpha = 0.0;
    for (int i = 0; i < casterCount; i++)
    {
        Caster caster = casters[i];
        float2 diff = abs(caster.pos.xz - inData.positionW.xz);
        float distSq = dot(diff, diff);
        float radiusSq = caster.radius * caster.radius;
        if(radiusSq > 0.0f)
        {
            // 距離の二乗を半径の二乗で正規化
            float casterAlpha = saturate(1.0f - distSq / radiusSq);
            shadowAlpha = max(casterAlpha, shadowAlpha);
        }
    }
    float4 shadowColor = float4(0, 0, 0, shadowAlpha);
    float4 color = diffuse * shade + diffuse * ambient + speculer;
        
    // 最終的な色
    return lerp(color,shadowColor,shadowAlpha);
}