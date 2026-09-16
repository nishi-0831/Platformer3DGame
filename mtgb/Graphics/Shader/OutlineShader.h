#pragma once
#include "IShader.h"
namespace mtgb
{
	class OutlineShader : public IShader
	{
		struct ConstantBuffer
		{
			Matrix4x4 g_matrixWorldViewProj; // ワールド・ビュー・プロジェクション行列
			Vector4 g_color;				 // アウトラインの色
			FLOAT g_scale;					 // アウトラインの長さ
			Vector3 g_padding;
		};

	  public:
		OutlineShader();
		void Initialize(ID3D11Device* _pDevice) override;
		// IShader を介して継承されました
		void Draw(ID3D11DeviceContext* _pCtx, const Transform& _transform, MeshAsset* _pAsset, int _frame) override;

	  private:
		Vector4 outlineColor_;
		float outlineScale_;
	};
} // namespace mtgb