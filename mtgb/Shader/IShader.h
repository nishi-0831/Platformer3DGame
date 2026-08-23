#pragma once
#include <functional>
#include <d3d11.h>
#include <unordered_map>
#include "DirectX11Draw.h"
#include "Screen.h"
#include "Components/Transform/Transform.h"
#include <wrl/client.h>
#include "Model/MeshAsset.h"
#include "ReflectiveConstantBuffer.h"
#include "Utility/StringComparators.h"

using Microsoft::WRL::ComPtr;

struct ID3D11Buffer;

namespace mtgb
{
	class IShader
	{
	  public:
		IShader();
		virtual ~IShader() {}

		/// <summary>
		/// シェーダの初期化処理
		/// </summary>
		virtual void Initialize(ID3D11Device* _pDevice) = 0;
		// virtual void Draw() = 0;
		virtual void Release();
		virtual void Draw(ID3D11DeviceContext* _pCtx, const Transform& _transform, MeshAsset* _pAsset, int _frame) = 0;
		void Bind(ID3D11DeviceContext* _pCtx);
		ReflectiveConstantBuffer* GetConstantBuffer(std::string_view _name);

	  protected:
		void InitializeCommonGpuResources(ID3D11Device* _pDevice, std::wstring_view _fileName);
		std::unordered_map<std::string, ReflectiveConstantBuffer, TransparentStringHash, TransparentStringEq>
			cBufferMap_;
		/// <summary>
		/// <para>入力レイアウト</para>
		/// <para></para>
		/// </summary>
		ComPtr<ID3D11InputLayout> pInputLayout_;

		/// <summary>
		/// <para>頂点シェーダ</para>
		/// <para>頂点の情報</para>
		/// </summary>
		ComPtr<ID3D11VertexShader> pVertexShader_;

		/// <summary>
		/// <para>ピクセルシェーダ</para>
		/// </summary>
		ComPtr<ID3D11PixelShader> pPixelShader_;

		/// <summary>
		/// <para>ラスタライザ</para>
		/// </summary>
		ComPtr<ID3D11RasterizerState> pRasterizerState_;

	  private:
		void ParseAndCreateConstantBuffer(ID3D11Device* _pDevice, ID3DBlob* _pBlob);
	};

} // namespace mtgb
