#pragma once
#include "IShader.h"

namespace mtgb
{
	class UVScrollShader : public IShader
	{
		struct TimeBuffer
		{
			float g_time;
			float g_padding[3];
		};

	  public:
		void Initialize(ID3D11Device* _pDevice) override;
		// IShader を介して継承されました
		void Draw(ID3D11DeviceContext* _pCtx, const Transform& _transform, MeshAsset* _pAsset, int _frame) override;

	  private:
		float time_;
	};
} // namespace mtgb