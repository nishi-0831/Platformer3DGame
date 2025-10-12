#include "Figure.h"
#include <DirectXMath.h>
#include <d3d11.h>
#include "DirectX11Draw.h"
#include "RectInt.h"
#include "RectF.h"
#include "Vector2F.h"
#include "Game.h"
#include "Screen.h"


mtgb::Figure::Figure()
{
}

mtgb::Figure::~Figure()
{
}

void mtgb::Figure::Draw(
	const RectInt& _rect,
	const Color& _color)
{
	Draw(RectF(_rect.point,_rect.size), _color);
}

void mtgb::Figure::Draw(const RectF& _rect, const Color& _color)
{
	using DirectX::XMMatrixScaling;      // �g�k
	using DirectX::XMMatrixTranspose;    // �s�Ɨ�����ւ���
	using DirectX::XMMatrixTranslation;  // ���s�ړ�
	using DirectX::XMMatrixIdentity;     // �P�ʍs��

	DirectX11Draw::SetBlendMode(BlendMode::Default);  // �u�����h���[�h�f�t�H���g
	DirectX11Draw::SetIsWriteToDepthBuffer(false);    // �[�x�o�b�t�@�ւ̏������݂Ȃ�

	IShader::Draw<ConstantBuffer, Vertex>(
		[&, this](ConstantBuffer* _pCB)
		{
			_pCB->g_color = _color.ToVector4Norm();
			_pCB->g_worldMatrix = XMMatrixIdentity();
#pragma region TODO: �v�Z�������K�v
			// �X�N���[���T�C�Y���擾
			const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

			// ���w���W�ƕ`����W��y�����ى���
			RectF cartesianBox{ _rect };
			cartesianBox.y = SCREEN_SIZE.y - cartesianBox.y;
			cartesianBox.height *= -1;

			const Vector2F VIEW_BEGIN{ cartesianBox.GetBegin() };
			const Vector2F VIEW_END{ cartesianBox.GetEnd() };

			// �\������T�C�Y�ɍ��킹��
			Matrix4x4 scalingBox = XMMatrixScaling(
				std::abs(VIEW_END.x - VIEW_BEGIN.x) * 2.0f,
				std::abs(VIEW_END.y - VIEW_BEGIN.y) * 2.0f,
				1.0f);

			// �\������{�b�N�X�̈ʒu���ړ�����
			Matrix4x4 moveBox = XMMatrixTranslation(
				((VIEW_END.x - VIEW_BEGIN.x) / 2.0f + VIEW_BEGIN.x) / (SCREEN_SIZE.x / 2.0f),
				((VIEW_BEGIN.y - VIEW_END.y) / 2.0f + VIEW_END.y) / (SCREEN_SIZE.y / 2.0f),
				0.0f);

			// ��ʂɍ��킹��
			Matrix4x4 scalingView = XMMatrixScaling(
				1.0f / (SCREEN_SIZE.x * 2),
				1.0f / (SCREEN_SIZE.y * 2),
				1.0f);

			// �I�t�Z�b�g - ��ʒ��S��(0, 0) ������(-1, -1)
			Matrix4x4 offsetView
			{
				XMMatrixTranslation(-1.0f, -1.0f, 0.0f)
			};

			// �ŏI�I�ȍs��
			Matrix4x4 world
			{
				scalingBox * scalingView * moveBox * offsetView
			};
#pragma endregion

			// MEMO: CPU -> �s�D��, GPU -> ��D��
			//  REF: https://qiita.com/niusounds/items/65099654673f5df3be9b
			_pCB->g_worldMatrix = XMMatrixTranspose(world);
		},
		[&, this](ID3D11DeviceContext* _pDC)
		{
		});
}

void mtgb::Figure::InitializeVertexBuffer(ID3D11Device* _pDevice)
{
	Vertex vertexes[]
	{
		Vector3{ -1,  1,  0 },  // ����
		Vector3{  1,  1,  0 },  // �E��
		Vector3{ -1, -1,  0 },  // ����
		Vector3{  1, -1,  0 },  // �E��
	};

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(vertexes),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA
	{
		.pSysMem = vertexes,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		&INITIALIZE_DATA,
		&pVertexBuffer_);

	massert(SUCCEEDED(hResult)  // ���_�o�b�t�@�̍쐬�ɐ���
		&& "���_�o�b�t�@�̍쐬�Ɏ��s @Figure::InitializeVertexBuffer");
}

void mtgb::Figure::InitializeIndexBuffer(ID3D11Device* _pDevice)
{
	static const int INDEXES[]{ 2, 1, 0, 2, 3, 1 };

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(INDEXES),
		.Usage = D3D11_USAGE_DEFAULT,
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA
	{
		.pSysMem = INDEXES,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		&INITIALIZE_DATA,
		&pIndexBuffer_);

	massert(SUCCEEDED(hResult)  // �C���f�b�N�X�o�b�t�@�̍쐬�ɐ���
		&& "�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s @Figure::InitializeIndexBuffer");
}

void mtgb::Figure::InitializeConstantBuffer(ID3D11Device* _pDevice)
{
	static const ConstantBuffer INITIALIZE_CONSTANT_BUFFER
	{
		.g_color = { 1, 0, 0, 1 },
		.g_worldMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity()),
	};

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(ConstantBuffer),
		.Usage = D3D11_USAGE_DYNAMIC,  // MEMO: �r���ŏ��������邽��dynamic
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA
	{
		.pSysMem = &INITIALIZE_CONSTANT_BUFFER,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		&INITIALIZE_DATA,
		&pConstantBuffer_);

	massert(SUCCEEDED(hResult)  // �R���X�^���g�o�b�t�@�̍쐬�ɐ���
		&& "�R���X�^���g�o�b�t�@�̍쐬�Ɏ��s @Figure::InitializeConstantBuffer");
}
