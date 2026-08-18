#pragma once
#include "IShader.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Vector4.h"
#include "Vector2.h"
#include "RectInt.h"
#include "Color.h"
#include <string>
#include "Texture2D.h"
#include "RectF.h"
#include "Vector2F.h"

namespace mtgb
{
	class Transform;

	class Sprite
	{
		/// <summary>
		/// 頂点情報
		/// </summary>
		struct Vertex
		{
			Vector4 position;
			Vector4 uv;
		};

		/// <summary>
		/// GPUに送るやつ
		/// </summary>
		struct ConstantBuffer
		{
			Matrix4x4 g_matrixCameraTranslate; // カメラ座標変換行列
			Matrix4x4 g_matrixCameraRotation;  // カメラ座標変換行列
			Matrix4x4 g_matrixWorldTranslate;  // 頂点座標変換行列
			Matrix4x4 g_matrixWorldRotation;   // 頂点座標変換行列
			Matrix4x4 g_matrixTexture;		   // テクスチャ座標変換行列
			Vector4 g_color;				   // 色
			Vector2 g_angle;				   // 角度
			float padding[2];
		};

	  public:
		Sprite();
		~Sprite();
		void Initialize();
		/// <summary>
		/// 画像を読み込む
		/// </summary>
		/// <param name="_fileName">画像のファイル名</param>
		void Load(const std::wstring& _fileName);
		/// <summary>
		/// Spriteを描画する
		/// </summary>
		/// <param name="_draw">描画する矩形</param>
		/// <param name="_cut">画像内のトリミング矩形</param>
		/// <param name="_rotationZ">回転角度(Degree)</param>
		/// <param name="_color">描画色</param>
		void Draw(const RectF& _draw, const float _rotationZ, const RectF& _cut, const Color& _color);

		/// <summary>
		/// 読み込まれた画像のサイズを取得
		/// </summary>
		/// <returns></returns>
		inline const Vector2Int GetSize() const
		{
			return texture2D_.GetSize();
		}

		inline std::wstring GetFileName()
		{
			return fileName_;
		}
		void InitializeVertexBuffer();
		void InitializeIndexBuffer();
		void InitializeConstantBuffer();

	  protected:
		ComPtr<ID3D11Buffer> pVertexBuffer_;   // 頂点の位置、色
		ComPtr<ID3D11Buffer> pIndexBuffer_;	   // ポリゴンを結ぶ順番
		ComPtr<ID3D11Buffer> pConstantBuffer_; // 　シェーダのグローバル変数
		/// <summary>
		/// <para>頂点レイアウト</para>
		/// <para></para>
		/// </summary>
		ComPtr<ID3D11InputLayout> pVertexLayout;

		/// <summary>
		/// <para>頂点シェーダ</para>
		/// <para>頂点の情報</para>
		/// </summary>
		ComPtr<ID3D11VertexShader> pVertexShader;

		/// <summary>
		/// <para>ピクセルシェーダ</para>
		/// <para></para>
		/// </summary>
		ComPtr<ID3D11PixelShader> pPixelShader;

		/// <summary>
		/// <para>ラスタライザ</para>
		/// <para>どのピクセルを光らせるかの情報</para>
		/// </summary>
		ComPtr<ID3D11RasterizerState> pRasterizerState;

	  private:
		void InitializeShader();
		Texture2D texture2D_;	// 2Dのテクスチャ
		std::wstring fileName_; // 読み込んだファイル
	};
} // namespace mtgb
