#pragma once
#include "IShader.h"

namespace mtgb
{
	/// <summary>
	/// 地形用のシェーダー。丸影を描画する
	/// </summary>
	class Box3DShader : public IShader
	{
	  public:
		void Initialize(ID3D11Device* _pDevice) override;
		// IShader を介して継承されました
		void Draw(ID3D11DeviceContext* _pCtx, const Transform& _transform, MeshAsset* _pAsset, int _frame) override;
	};
} // namespace mtgb