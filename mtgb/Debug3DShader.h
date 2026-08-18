#pragma once
#include "IShader.h"

namespace mtgb
{
	class Debug3DShader : public IShader
	{
	  public:
		void Initialize(ID3D11Device* _pDevice) override;
		// IShader を介して継承されました
		void Draw(ID3D11DeviceContext* _pCtx, const Transform& _transform, MeshAsset* _pAsset, int _frame) override;
	};
} // namespace mtgb