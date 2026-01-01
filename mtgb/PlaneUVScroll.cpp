#include "stdafx.h"
#include "PlaneUVScroll.h"
#include "DirectX11Draw.h"
#include "ShaderType.h"
#include "Game.h"
#include "CameraSystem.h"
#include "Transform.h"
#include "GameTime.h"
#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

namespace
{
	Vector4 kLightDir{0, 1, 1, 0};
}

mtgb::PlaneUVScroll::PlaneUVScroll()
	: time_{0.0f}
{
}

mtgb::PlaneUVScroll::~PlaneUVScroll()
{
}

void mtgb::PlaneUVScroll::Initialize()
{
	IShader::Initialize();
}

void mtgb::PlaneUVScroll::Release()
{
	pTimeConstantBuffer_.Reset();
	IShader::Release();
}

void mtgb::PlaneUVScroll::LoadTexture(const std::wstring& _fileName)
{
	texture_.Load(_fileName);
}

void mtgb::PlaneUVScroll::Draw(const Transform& _transform)
{
	DirectX11Draw::SetIsWriteToDepthBuffer(true);
	DirectX11Draw::SetShader(ShaderType::Sea);

	time_ += Time::DeltaTimeF() * 0.5f;

	const CameraSystem& camera = Game::System<CameraSystem>();

	Matrix4x4 mWorld{};
	_transform.GenerateWorldMatrix(&mWorld);

	Matrix4x4 mView{};
	camera.GetViewMatrix(&mView);

	Matrix4x4 mProj{};
	camera.GetProjMatrix(&mProj);

	IShader::Draw<ConstantBuffer, Vertex>(
		[&](ConstantBuffer* _pCB)
		{
			_pCB->g_matrixWorldViewProj = XMMatrixTranspose(mWorld * mView * mProj);
			_pCB->g_matrixWorld			= XMMatrixTranspose(mWorld);

			XMMATRIX rotateX		  = XMMatrixRotationX(XMConvertToRadians(_transform.rotate.f[0]));
			XMMATRIX rotateY		  = XMMatrixRotationY(XMConvertToRadians(_transform.rotate.f[1]));
			XMMATRIX rotateZ		  = XMMatrixRotationZ(XMConvertToRadians(_transform.rotate.f[2]));
			XMMATRIX matRotate		  = rotateZ * rotateY * rotateX;
			XMMATRIX matScale		  = XMMatrixScaling(_transform.scale.x, _transform.scale.y, _transform.scale.z);
			_pCB->g_matrixNormalTrans = XMMatrixTranspose(matRotate * XMMatrixInverse(nullptr, matScale));

			_pCB->g_lightDirection = kLightDir;
			_pCB->g_diffuse		   = Vector4{1, 1, 1, 1};
			_pCB->g_ambient		   = Vector4{1, 1, 1, 1};
			_pCB->g_speculer	   = Vector4{0, 0, 0, 0};
			_pCB->g_shininess	   = 0.0f;
			camera.GetPosition(&_pCB->g_cameraPosition);
			_pCB->g_isTexture	 = TRUE;
			_pCB->g_textureScale = Vector4(_transform.scale.x, _transform.scale.y, _transform.scale.z, 0.0f);
		},
		[&](ID3D11DeviceContext* _pContext)
		{
			_pContext->VSSetConstantBuffers(1, 1, pTimeConstantBuffer_.GetAddressOf());
			_pContext->PSSetConstantBuffers(1, 1, pTimeConstantBuffer_.GetAddressOf());

			// UVスクロールに使うデルタタイムをセット
			D3D11_MAPPED_SUBRESOURCE mapped{};
			if (SUCCEEDED(_pContext->Map(pTimeConstantBuffer_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
			{
				TimeBuffer tb{};
				tb.g_time = time_;
				memcpy_s(mapped.pData, mapped.RowPitch, &tb, sizeof(tb));
				_pContext->Unmap(pTimeConstantBuffer_.Get(), 0);
			}

			// テクスチャセット
			ID3D11SamplerState* pSampler   = texture_.GetSamplerState();
			ID3D11ShaderResourceView* pSrv = texture_.GetShaderResourceView();
			_pContext->PSSetSamplers(0, 1, &pSampler);
			_pContext->PSSetShaderResources(0, 1, &pSrv);
		},
		6
	);
}

void mtgb::PlaneUVScroll::InitializeVertexBuffer(ID3D11Device* _pDevice)
{
	// XY平面の板ポリ(中心原点)・表向き+Z
	Vertex v[4]{};
	v[0].position = {-0.5f, 0.0f, -0.5f};
	v[0].normal	  = {0, 1, 0};
	v[0].uv		  = {0.0f, 1.0f};
	v[1].position = {-0.5f, 0.0f, 0.5f};
	v[1].normal	  = {0, 1, 0};
	v[1].uv		  = {0.0f, 0.0f};
	v[2].position = {0.5f, 0.0f, 0.5f};
	v[2].normal	  = {0, 1, 0};
	v[2].uv		  = {1.0f, 0.0f};
	v[3].position = {0.5f, 0.0f, -0.5f};
	v[3].normal	  = {0, 1, 0};
	v[3].uv		  = {1.0f, 1.0f};

	const D3D11_BUFFER_DESC desc{
		.ByteWidth			 = sizeof(v),
		.Usage				 = D3D11_USAGE_DEFAULT,
		.BindFlags			 = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags		 = 0,
		.MiscFlags			 = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA init{
		.pSysMem		  = v,
		.SysMemPitch	  = 0,
		.SysMemSlicePitch = 0,
	};

	_pDevice->CreateBuffer(&desc, &init, pVertexBuffer_.ReleaseAndGetAddressOf());
}

void mtgb::PlaneUVScroll::InitializeIndexBuffer(ID3D11Device* _pDevice)
{
	const uint32_t indices[6] = {0, 1, 2, 0, 2, 3};

	const D3D11_BUFFER_DESC desc{
		.ByteWidth			 = sizeof(indices),
		.Usage				 = D3D11_USAGE_DEFAULT,
		.BindFlags			 = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags		 = 0,
		.MiscFlags			 = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA init{
		.pSysMem		  = indices,
		.SysMemPitch	  = 0,
		.SysMemSlicePitch = 0,
	};

	_pDevice->CreateBuffer(&desc, &init, pIndexBuffer_.ReleaseAndGetAddressOf());
}

void mtgb::PlaneUVScroll::InitializeConstantBuffer(ID3D11Device* _pDevice)
{
	{
		const D3D11_BUFFER_DESC desc{
			.ByteWidth			 = sizeof(ConstantBuffer),
			.Usage				 = D3D11_USAGE_DYNAMIC,
			.BindFlags			 = D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags		 = D3D11_CPU_ACCESS_WRITE,
			.MiscFlags			 = 0,
			.StructureByteStride = 0,
		};

		_pDevice->CreateBuffer(&desc, nullptr, pConstantBuffer_.ReleaseAndGetAddressOf());
	}

	{
		const D3D11_BUFFER_DESC desc{
			.ByteWidth			 = sizeof(TimeBuffer),
			.Usage				 = D3D11_USAGE_DYNAMIC,
			.BindFlags			 = D3D11_BIND_CONSTANT_BUFFER,
			.CPUAccessFlags		 = D3D11_CPU_ACCESS_WRITE,
			.MiscFlags			 = 0,
			.StructureByteStride = 0,
		};

		_pDevice->CreateBuffer(&desc, nullptr, pTimeConstantBuffer_.ReleaseAndGetAddressOf());
	}
}
