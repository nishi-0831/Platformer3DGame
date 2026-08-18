cbuffer outline : register(b0)
{
    float4x4 matWVP;
    float4 color;
    float scale;
    float3 padding;
};

float4 VS(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD) : SV_Position
{
    //ピクセルシェーダーへ渡す情報
    float4 outData;

    normal.w = 0;
    
    position.xyz = position * scale;
    position = mul(position, matWVP);
    
    return position;
}
struct VS_OUT
{
                 //セマンティクス
    float4 pos : SV_POSITION; //位置
    float4 normal : NORMAL0; // 法線
    float2 uv : TEXCOORD; //UV座標
    float4 eye : NORMAL1;
    float4 posw : POSITION0;
};

//───────────────────────────────────────
// ピクセルシェーダ
//───────────────────────────────────────
float4 PS(VS_OUT input) : SV_Target
{
    return color;
}