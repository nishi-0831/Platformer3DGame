#include "3DCommon.hlsli"

cbuffer Time : register(b1)
{
    float2 g_scroll_dir;
    float2 g_scroll_speed;
    float g_time;
    float3 g_time_padding_01;
    float4 g_texture_scale;
    bool g_reverse_uv;
    float3 g_time_padding_02;
}

struct VS_OUT_UV_SCROLL
{
    float4 position : SV_POSITION; // 位置
    float4 normal : NORMAL0; // 法線
    float2 uv : TEXCOORD; // uv座標
    float4 eye : NORMAL1;
};

float Rand(float seed)
{
    float a = frac(sin(seed) * 43758.5453); // 疑似乱数生成
    return a;
}

/*
* 頂点シェーダ
*/
VS_OUT VS(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
    VS_OUT outData;

    float4 pos = position;
    outData.position = mul(pos, g_matrixWVP);
   
    // 法線の変形
    normal.w = 0;
    outData.normal = mul(normal, g_matrixNormalTrans);

    float4 worldPosition = mul(pos, g_matrixW);
    // 視線ベクトル
    outData.eye = normalize(g_cameraPosition - worldPosition);
    
    // 法線の絶対値を取得
    float3 absNormal = abs(outData.normal.xyz);
    
    outData.uv = uv;
    
    // UV座標
    
    return outData;
}

float4 PS(VS_OUT input) : SV_Target
{
    // 光源方向
    float4 lightDir = normalize(g_lightDir);
    
    // 法線
    input.normal = normalize(input.normal);
    
    // 
    float4 shade = saturate(dot(input.normal, -lightDir));
    shade.a = 1;
    
    
    float4 diffuse;
    if (g_hasTexture == true)
    {
        float offsetX = g_time * g_scroll_speed.x;
        float offsetY = g_time * g_scroll_speed.y;
        float2 scroll_dir = normalize(g_scroll_dir);
        
        float2 scrolledUV = input.uv;
        scrolledUV.x += offsetX * scroll_dir.x;
        scrolledUV.x *= g_texture_scale.x;
        scrolledUV.y += offsetY * scroll_dir.y;
        scrolledUV.y *= g_texture_scale.z;
    
        if (g_reverse_uv == true)
        {
            scrolledUV = float2(1.0 - scrolledUV.x, 1.0 - scrolledUV.y);
        }
       
        diffuse = g_texture.Sample(g_sampler, scrolledUV);
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
        float4 r = reflect(lightDir, input.normal);
        specuer = pow(saturate(dot(r, input.eye)), g_shuniness) * g_speculerColor;
    }
    
    // 最終的な色
    
    float4 color = diffuse * shade + diffuse * ambient + specuer;
    
    return color;
}