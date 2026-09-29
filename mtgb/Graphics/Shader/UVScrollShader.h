#pragma once
#include "IShader.h"

namespace mtgb
{
	/// <summary>
	/// UVをスクロールさせるシェーダー
	/// </summary>
	class UVScrollShader : public IShader
	{
	  public:
		struct TimeBuffer
		{
			Vector2 g_sroll_dir;
			Vector2 g_scroll_speed;
			float g_time;
			Vector3 g_padding;
			Vector4 g_texture_scale;
			int g_reverse_uv;
			Vector3 g_padding_2;
		};
		void Initialize(ID3D11Device* _pDevice) override;
		// IShader を介して継承されました
		void Draw(ID3D11DeviceContext* _pCtx, const Transform& _transform, MeshAsset* _pAsset, int _frame) override;
	};
} // namespace mtgb