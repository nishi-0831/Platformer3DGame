#include "3DCommon.hlsli"

//定義
#define MAX_BONE_MATRICES 128

cbuffer BoneMatrices : register(b1) //ボーンのポーズ行列が入る
{
	matrix g_boneMatrices[MAX_BONE_MATRICES];
};

//スキニング後の頂点・法線が入る
struct Skin
{
	float4 position;
	float3 normal;
};

//頂点バッファーの入力
struct VSSkinIn
{
	float4 position : POSITION; //位置   
	float4 normal : NORMAL; //頂点法線
	float2 uv : TEXCOORD; //テクスチャー座標
	uint4 boneIndex : BONE_INDEX; //ボーンのインデックス
	float4 boneWeight : BONE_WEIGHT; //ボーンの重み
};

//ピクセルシェーダーの入力(頂点バッファーの出力)
struct PSSkinIn
{
	float4 position : SV_Position; //位置
	float4 normal : NORMAL0; //頂点法線
	float2 uv : TEXCOORD; //テクスチャー座標
	float4 worldPosition : NORMAL1; //ワールド座標
	float4 eye : NORMAL2; //視線ベクトル
};

//頂点をスキニングする。頂点シェーダーで使用
Skin SkinVert(VSSkinIn input)
{
	Skin Output = (Skin) 0;
	float4 pos = input.position;
	float3 normal = input.normal.xyz;
	
	// スキンメッシュの場合
	if (any(input.boneWeight > 0.0f))
	{
		Output.position = float4(0, 0, 0, 0);
		Output.normal = float3(0, 0, 0);
		
        for (int i = 0; i < 4; i++)
        {
            if (input.boneWeight[i] > 0.0f)
            {
				// i番目のボーンの変換行列を取得
                matrix boneMatrix = g_boneMatrices[input.boneIndex[i]];
		
				// 頂点座標を変換
                Output.position += input.boneWeight[i] * mul(pos, boneMatrix);
		
				// 法線も変換(回転のみ適用)
                Output.normal += input.boneWeight[i] * mul(normal, (float3x3) boneMatrix);
            }
        }
	}
	else
	{
		// スキンメッシュでない場合は元の頂点をそのまま使う
		Output.position = pos;
		Output.normal = normal;
	}
	
	return Output;
}

// 頂点シェーダー
PSSkinIn VS(VSSkinIn input)
{
	PSSkinIn output;
	
	Skin vSkinned = SkinVert(input);

	output.position = mul(vSkinned.position, g_matrixWVP);
	output.normal = mul(float4(vSkinned.normal, 0), g_matrixNormalTrans);
	output.uv = input.uv.xy;
	
	output.worldPosition = mul(vSkinned.position, g_matrixW);
	
	// 視線ベクトル
	output.eye = normalize(g_cameraPosition - output.worldPosition);
	
	return output;
}

//ピクセルシェーダー
float4 PS(PSSkinIn inData) : SV_Target
{
	// 光源方向
	float4 lightDir = normalize(g_lightDir);
	
	// 法線
	inData.normal = normalize(inData.normal);
	
	// 拡散反射の計算
	float4 shade = saturate(dot(inData.normal, -lightDir));
	shade.a = 1; // 透明度は操作したくないため、強制的にアルファ値1
	
	float4 diffuse;
	if (g_hasTexture == true)
	{
		// テクスチャ
		diffuse = g_texture.Sample(g_sampler, inData.uv);
	}
	else
	{
		// 拡散反射成分
		diffuse = g_diffuseColor;
	}
	
	// 環境光
	float4 ambient = float4(1, 1, 1, 1);
	
	// 鏡面反射成分
	float4 specuer = float4(0, 0, 0, 0);
	if (g_speculerColor.a != 0)
	{
		// 正反射ベクトル
		float4 r = reflect(lightDir, inData.normal);
		// 鏡面反射成分計算
		specuer = pow(saturate(dot(r, inData.eye)), g_shuniness) * g_speculerColor;
	}
	
	// 最終的な色
	float4 color = diffuse * shade + diffuse * ambient + specuer;
	return color;
}