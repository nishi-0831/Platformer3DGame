#pragma once
#include "Vector2.h"
#include "Matrix4x4.h"
#include "Vector4.h"
#include "IncludingWindows.h"
namespace mtgb
{
	struct ConstantBuffer
	{
		Matrix4x4 g_matrixWorldViewProj; // ワールド・ビュー・プロジェクション行列
		Matrix4x4 g_matrixNormalTrans;	 // 回転行列と拡大行列の逆行列
		Matrix4x4 g_matrixWorld;		 // ワールド行列
		Vector4 g_lightDirection;		 // ライトの向き
		Vector4 g_diffuse;				 // 光があたったときへの拡散反射光(マテリアル色)
		Vector4 g_ambient;				 // 全体的な環境光 (光が当たらない場所にも明るく)
		Vector4 g_speculer;				 // 鏡面反射 (Lambertの場合は0)
		Vector4 g_cameraPosition;		 // カメラの位置（ハイライトの計算に必要）
		FLOAT g_shininess;				 // スペキュラの強さ
		BOOL g_isTexture;				 // テクスチャの有無
		Vector2 g_padding;
		Vector4 g_textureScale;
	};
} // namespace mtgb