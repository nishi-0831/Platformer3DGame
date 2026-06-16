#include "3DCommon.hlsli"

cbuffer ShadowParam : register(b1)
{
    float4 casterPos;
    float softness;
    float playerHeight;
    float2 shadowPadding;
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
    
    // 
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
    float4 specuer = float4(0, 0, 0, 0);
    if (g_speculerColor.a != 0)
    {
        float4 r = reflect(lightDir, inData.normal);
        specuer = pow(saturate(dot(r, inData.eye)), g_shuniness) * g_speculerColor;
    }
    
    // 影
    float2 casterPosXZ = casterPos.xz;
    float2 posXZ = inData.positionW.xz;
    float2 diff = abs(casterPosXZ - posXZ);
    // 距離の二乗
    float distSq = dot(diff, diff);
    
    float ratio = 1.0f;
    float radius = 1.0f;
    float alpha = 0.5f;
    
    float shadowAlpha = saturate((radius * radius - distSq) * softness);
    float4 shadowColor = float4(0, 0, 0, shadowAlpha);
    // 最終的な色
    float4 color = diffuse * shade + diffuse * ambient + specuer;
    
    //return float4(shadowAlpha, 0, 0, 255);
    
    return lerp(color,shadowColor,shadowAlpha);
}