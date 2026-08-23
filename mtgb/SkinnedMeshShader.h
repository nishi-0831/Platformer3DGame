#pragma once
#include "IShader.h"

class SkinnedMeshShader : public IShader
{
  public:
	void Initialize(ID3D11Device* _pDevice) override;
	// IShader を介して継承されました
	void Draw(ID3D11DeviceContext* _pCtx, const Transform& _transform, MeshAsset* _pAsset, int _frame) override;

  private:
	/// <summary>
	/// フレーム数をもとにボーン行列を計算、セットする
	/// </summary>
	/// <param name="_pCtx"></param>
	/// <param name="_pAsset"></param>
	/// <param name="_frame"></param>
	void SetBoneMatrix(ID3D11DeviceContext* _pCtx, MeshAsset* _pAsset, int _frame);
};