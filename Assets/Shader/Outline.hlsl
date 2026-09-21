cbuffer outline : register(b0)
{
    float4x4 matWVP;
    float4 color;
    float scale;
    float3 padding;
};

float4 VS(float4 position : POSITION) : SV_Position
{    
    position.xyz = position * scale;
    position = mul(position, matWVP);   
    return position;
}

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(float4 position : SV_POSITION) : SV_Target
{
    return color;
}