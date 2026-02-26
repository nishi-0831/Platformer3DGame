#pragma once
#include "IShader.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Color.h"
#include "Vector2.h"
#include "Texture2D.h"
#include <wrl/client.h>

namespace mtgb
{
	class Transform;

	class PlaneUVScroll : public IShader
	{
	  public:
		/// <summary>
		/// 頂点情報
		/// </summary>
		struct Vertex
		{
			Vector3 position; // 座標
			Vector3 normal;	  // 法線
			Vector2 uv;		  // uv座標
		};

		/// <summary>
		/// GPUに送るやつ
		/// </summary>
		struct ConstantBuffer
		{
			Matrix4x4 g_matrixWorldViewProj; // ワールド・ビュー・プロジェクション行列
			Matrix4x4 g_matrixNormalTrans;	 // 回転行列と拡大行列の逆行列
			Matrix4x4 g_matrixWorld;		 // ワールド行列
			Matrix4x4 g_matrixViewProj;		 // ビュー・プロジェクション行列
			Vector4 g_lightDirection;		 // ライトの向き
			Vector4 g_diffuse;				 // 拡散反射
			Vector4 g_ambient;				 // 環境光
			Vector4 g_speculer;				 // 鏡面反射
			Vector4 g_cameraPosition;		 // カメラ位置
			FLOAT g_shininess;				 // スペキュラ強さ
			BOOL g_isTexture;				 // テクスチャ有無
			Vector2 g_padding;
			Vector4 g_textureScale;
		};

		struct TimeBuffer
		{
			float g_time;
			float g_padding[3];
		};

	  public:
		PlaneUVScroll();
		~PlaneUVScroll();

		void Initialize() override;
		void Release() override;

		void LoadTexture(const std::wstring& _fileName);

		void Draw(const Transform& _transform);

	  private:
		void InitializeVertexBuffer(ID3D11Device* _pDevice) override;
		void InitializeIndexBuffer(ID3D11Device* _pDevice) override;
		void InitializeConstantBuffer(ID3D11Device* _pDevice) override;

	  private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pTimeConstantBuffer_;
		Texture2D texture_;
		float time_;
	};
} // namespace mtgb
