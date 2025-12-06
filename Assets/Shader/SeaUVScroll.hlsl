#include "2DCommon.hlsli"

cbuffer Time : register(b1)
{
    float g_time;
    float g_padding[3];
}

/*
* 頂点シェーダ
*/
VSOutput VS(float4 position : POSITION, float4 uv : TEXCOORD)
{
    VSOutput output;
    output.position = mul(position, g_matrixWorldRotation);
    output.position = mul(output.position, g_matrixCameraRotation);
    output.position = mul(output.position, g_matrixWorldTranslate);
    output.position = mul(output.position, g_matrixCameraTranslate);
    output.uv = mul(uv, g_matrixTexture);
    //output.g_position = position.xy;

    return output;
}

float4 PS(VSOutput input) : SV_Target
{
    float scrollSpeedX = 0.05f;
    float scrollSpeedY = 0.0f;
    
    float offsetX = g_time * scrollSpeedX;
    float offsetY = g_time * scrollSpeedY;
    
    float2 scrolledUV = input.uv;
    scrolledUV.x += offsetX;
    scrolledUV.y += offsetY;
    
    float4 color = g_texture.Sample(g_sampler, scrolledUV);
    return color;
}