// #include "3DCommon.hlsli"

Texture2D g_texture : register(t0); // テクスチャ
SamplerState g_sampler : register(s0); // テクスチャのサンプラ

cbuffer global : register(b0)
{
    matrix g_matrixWVP; // ワールド・ビュー・プロジェクションの合成行列
    matrix g_matrixNormalTrans; // 法線の変換行列 (回転行列)
    matrix g_matrixW; // ワールド変換行列
    matrix g_matrixVP;
    float4 g_lightDir; // 環境光線
    float4 g_diffuseColor; // マテリアルの色
    float4 g_ambientColor; // 影の色
    float4 g_speculerColor; // スペキュラーからの色
    float4 g_cameraPosition; // 視点 (カメラの座標)
    float g_shuniness; // ハイライトの強さ
    bool g_hasTexture; // テクスチャを持っているか
    float2 padding;
    float4 g_textureScale;
};

cbuffer Time : register(b1)
{
    float g_time;
    float g_padding[3];
}

struct VS_OUT
{
    float4 position : SV_POSITION; // 位置
    float4 normal : TEXCOORD0; // 法線
    float2 uv : TEXCOORD1; // uv座標
    float4 eye : TEXCOORD2;
};

struct GS_IN
{
    float4 position : POSITION0; // 位置
    float4 normal : TEXCOORD0; // 法線
    float2 uv : TEXCOORD1; // uv座標
    float4 eye : TEXCOORD2;
};

VS_OUT GsInToVsOut(GS_IN gs)
{
    VS_OUT output;
    
    output.position = gs.position;
    // output.position = mul(gs.position, g_matrixVP);
    output.normal = gs.normal;
    output.uv = gs.uv;
    output.eye = gs.eye;
    return output;
}

/*
* 頂点シェーダ
*/
GS_IN VS(float4 position : POSITION, float4 normal : NORMAL, float2 uv : TEXCOORD)
{
    GS_IN outData;

    float4 pos = position;
    // これは問題なし
    outData.position = mul(pos, g_matrixW);
    outData.position = mul(outData.position, g_matrixVP);

    // 法線の変形
    normal.w = 0;
    outData.normal = mul(normal, g_matrixNormalTrans);

    float4 worldPosition = mul(pos, g_matrixW);
    // 視線ベクトル
    outData.eye = normalize(g_cameraPosition - worldPosition);
    
    // 法線の絶対値を取得
    float3 absNormal = abs(outData.normal.xyz);
    
    float2 selectedScale;
    
    // XZ軸のスケールを使用
    // UV座標
    outData.uv = uv;
    
    return outData;
}

// MEMO:
// triangle : 三角形のリスト、またはストリップ
// TriangleStream
// - ストリーム出力オブジェクト。ジオメトリシェーダは計算結果をこれに渡す
// - 三角形の頂点を受け取る型
[maxvertexcount(72)]
void GS(triangle GS_IN input[3], inout TriangleStream<VS_OUT> output)
{
    int appendCount = 0;
    
    // コンスタントバッファにVP行列も追加して、頂点シェーダでワールド変換、ジオメトリシェーダでVPで変換
    
    VS_OUT vert0 = GsInToVsOut(input[0]);

    output.Append(vert0);
    VS_OUT vert1 = GsInToVsOut(input[1]);
    output.Append(vert1);
    output.Append(GsInToVsOut(input[2]));
        
     //GS_IN vert = input[2];
     //vert.position.x = lerp(input[0].position.x, input[2].position.x, 0.5);
     //vert.position.y = input[2].position.y;
     //vert.uv.x = lerp(input[0].uv.x, input[2].uv.x, 0.5);
     //vert.eye.x = lerp(input[0].eye.x, input[2].eye.x, 0.5);
     //vert.eye.x = lerp(input[0].eye.x, input[2].eye.x, 0.5);
     //
     //VS_OUT newVert = GsInToVsOut(vert);
     //output.Append(GsInToVsOut(input[2]));
     //output.Append(newVert);
     //output.Append(vert0);
     output.RestartStrip();
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
        float scrollSpeedX = 0.05f;
        float scrollSpeedY = 0.0f;
    
        float offsetX = g_time * scrollSpeedX;
        float offsetY = g_time * scrollSpeedY;
    
        float2 scrolledUV = input.uv;
        scrolledUV.x += offsetX;
        scrolledUV.y += offsetY;
    
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