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
#include <d3dcompiler.h>
#include "HLSLInclude.h"
#include <vector>
using namespace DirectX;

namespace
{
	int indexCount = 0;
	int subCount = 5;
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
			_pCB->g_matrixViewProj		= XMMatrixTranspose(mView * mProj);

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
		indexCount
	);
	DirectX11Draw::pContext_->GSSetShader(nullptr, nullptr, 0);
}

void mtgb::PlaneUVScroll::InitializeVertexBuffer(ID3D11Device* _pDevice)
{
	// XY平面の板ポリ(中心原点)・表向き+Z
	std::vector<PlaneUVScroll::Vertex> vertices;
	for (int z = 0; z < subCount + 1; z++)
	{
		for (int x = 0; x < subCount + 1; x++)
		{
			PlaneUVScroll::Vertex vert;
			vert.position.x = (1.0f / subCount) * x;
			vert.position.y = 0.0f;
			vert.position.z = (1.0f / subCount) * z;
			vert.position.x -= 0.5f;
			vert.position.z -= 0.5f;

			vert.uv.x = ( (1.0f / subCount) * x);
			vert.uv.y = 1.0f - ((1.0f / subCount) * z);

			vert.normal = Vector3(0.0f, 1.0f, 0.0f);

			vertices.push_back(vert);
		}
	}

	UINT width = static_cast<UINT>(vertices.size() * sizeof(Vertex));
	D3D11_BUFFER_DESC desc
	{
		.ByteWidth			 = width,
		.Usage				 = D3D11_USAGE_DEFAULT,
		.BindFlags			 = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags		 = 0,
		.MiscFlags			 = 0,
		.StructureByteStride = 0,
	};

	D3D11_SUBRESOURCE_DATA init
	{
		.pSysMem		  = vertices.data(),
		.SysMemPitch	  = 0,
		.SysMemSlicePitch = 0,
	};

	_pDevice->CreateBuffer(&desc, &init, pVertexBuffer_.ReleaseAndGetAddressOf());
}

void mtgb::PlaneUVScroll::InitializeIndexBuffer(ID3D11Device* _pDevice)
{

	std::vector<UINT> indices;
	for (int z = 0; z < subCount; z++)
	{
		for (int x = 0; x < subCount; x++)
		{
			int idx0 = x + (subCount + 1) * z;
			int idx1 = idx0 + 1;
			int idx2 = idx0 + subCount + 1;
			int idx3 = idx2 + 1;

			indices.push_back(idx0);
			indices.push_back(idx3);
			indices.push_back(idx1);

			indices.push_back(idx0);
			indices.push_back(idx2);
			indices.push_back(idx3);
		}
	}
	indexCount = static_cast<int>(indices.size());
	D3D11_BUFFER_DESC desc
	{
		.ByteWidth			 = static_cast<UINT>(indices.size() * sizeof(UINT)),
		.Usage				 = D3D11_USAGE_DEFAULT,
		.BindFlags			 = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags		 = 0,
		.MiscFlags			 = 0,
		.StructureByteStride = 0,
	};

	D3D11_SUBRESOURCE_DATA init
	{
		.pSysMem		  = indices.data(),
		.SysMemPitch	  = 0,
		.SysMemSlicePitch = 0,
	};

	_pDevice->CreateBuffer(&desc, &init, pIndexBuffer_.ReleaseAndGetAddressOf());
}

void mtgb::PlaneUVScroll::InitializeConstantBuffer(ID3D11Device* _pDevice)
{
	{
		const D3D11_BUFFER_DESC desc
		{
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
		const D3D11_BUFFER_DESC desc
		{
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
