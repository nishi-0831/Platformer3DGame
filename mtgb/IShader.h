#pragma once
#include <functional>
#include <d3d11.h>
#include <unordered_map>
#include "DirectX11Draw.h"
#include "Screen.h"
#include "Transform.h"
#include <wrl/client.h>
#include "MeshAsset.h"
#include "ReflectiveConstantBuffer.h"
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
		ReflectiveConstantBuffer* GetConstantBuffer(const std::string& _name);

	  protected:
		void InitializeCommonGpuResources(ID3D11Device* _pDevice, std::wstring_view _fileName);
		// ComPtr<ID3D11Buffer> pConstantBuffer_;
		std::unordered_map<std::string, ReflectiveConstantBuffer> cBufferMap_;
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
	};

} // namespace mtgb
