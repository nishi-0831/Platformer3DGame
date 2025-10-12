#include "2DCommon.hlsli"

/*
* ���_�V�F�[�_
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

/*
* �s�N�Z���V�F�[�_
*/
float4 PS(VSOutput input) : SV_Target
{
    float4 color = g_color * g_texture.Sample(g_sampler, input.uv);
    //return fwidth(color.a);
    //return fwidth(color);  // MEMO: ���V�� abs(ddx(color)) + abs(ddy(color))
    return color;
}
