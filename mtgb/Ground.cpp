#include "Ground.h"
#include "MTAssert.h"
#include "CameraSystem.h"
#include "ReleaseUtility.h"
#include <vector>

namespace
{
	// �ȈՓI���C�g�̌���
	const mtgb::Vector4 LIGHT_DIRECTION{ 0.0f, 0.0f, 1.0f, 0.0f };
	// �ȈՓI�n�`�̃��[���h���W
	const mtgb::Vector3 WORLD_POSITION{ 0, -10, 0 };
}

mtgb::Ground::Ground(FbxNode* _pParent) :
	vertexCount_{ 0 },
	pVertexes_{ nullptr }
{
	if (_pParent != nullptr)
	{
		pNode_ = _pParent;
		pMesh_ = _pParent->GetMesh();
	}
}

mtgb::Ground::~Ground()
{
}

void mtgb::Ground::Initialize()
{
	vertexCount_ = pMesh_->GetControlPointsCount();
	polygonCount_ = pMesh_->GetPolygonCount();
	polygonVertexCount_ = pMesh_->GetPolygonVertexCount();
	IShader::Initialize();
}

void mtgb::Ground::Release()
{
	SAFE_DELETE_ARRAY(pVertexes_);
	SAFE_DELETE_ARRAY(pIndexData_);
}

void mtgb::Ground::Draw()
{
	using namespace DirectX;

	// �J�����V�X�e���ւ̃A�N�Z�X�p
	const CameraSystem& CAMERA{ Game::System<CameraSystem>() };

	IShader::Draw<ConstantBuffer, Vertex>(
		[&, this](ConstantBuffer* _pCB)
		{
			CAMERA.GetPosition(&_pCB->g_cameraPosition);
			Vector3 cameraWorldPos{ CAMERA.GetTransform().GetWorldPosition() };
			
			// TODO: �n�`�ʒu��ʃN���X���瑀��
			Matrix4x4 mWorld{ XMMatrixTranslationFromVector(cameraWorldPos + WORLD_POSITION) };
			//_transform.GenerateWorldMatrix(&mWorld);

			Matrix4x4 mView{};  // �r���[�s��
			CAMERA.GetViewMatrix(&mView);

			Matrix4x4 mProj{};  // �v���W�F�N�V�����s��
			CAMERA.GetProjMatrix(&mProj);

			_pCB->g_matrixWorldViewProj = XMMatrixTranspose(mView * mProj);
			//_pCB->g_matrixWorld = XMMatrixTranspose(mWorld);

			// TODO: ���C�g�̌�����LightSystem���瑀��
			_pCB->g_lightDirection = LIGHT_DIRECTION; // ���C�g�̌���
			_pCB->g_isTexture = (false);

		},
		[this](ID3D11DeviceContext* _pDC)
		{
		},
		GetIndexCount());
}

void mtgb::Ground::InitializeVertexBuffer(ID3D11Device* _pDevice)
{
	// xyz�A�N�Z�X�p�C���f�b�N�X
	enum { X, Y, Z };

	pVertexes_ = new Vertex[vertexCount_]{};

	int vertexCount{ pMesh_->GetControlPointsCount() };
	FbxVector4* pPoints{ pMesh_->GetControlPoints() };
	for (int i = 0; i < vertexCount; i++)
	{
		pVertexes_[i].position =
		{
			static_cast<float>(pPoints[i][X]),
			static_cast<float>(pPoints[i][Y]),
			static_cast<float>(pPoints[i][Z]),
		};
		pVertexes_[i].normal = Vector3::Up();
	}

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(Vertex) * vertexCount_,
		.Usage = D3D11_USAGE_DYNAMIC,                // �r���ŏ��������邽�� dynamic
		.BindFlags = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,    // �r���ŏ��������邩��
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA
	{
		.pSysMem = pVertexes_,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		&INITIALIZE_DATA,
		&pVertexBuffer_);

	massert(SUCCEEDED(hResult)
		&& "���_�o�b�t�@�̍쐬�Ɏ��s @FbxParts::InitializeVertexBuffer");
}

void mtgb::Ground::InitializeIndexBuffer(ID3D11Device* _pDevice)
{
	pIndexData_ = new DWORD[GetIndexCount()]{};
	//std::vector<DWORD> indexData{};
	//indexData.reserve()

	// �|���S�������Ă���
	for (uint32_t poly = 0; poly < polygonCount_; poly++)
	{
		int polygonSize{ pMesh_->GetPolygonSize(poly) };
		massert(polygonSize == 3 && "Fbx���f�����O�p������Ă��Ȃ� @Ground::InitializeIndexBuffer");

		// ���_�����Ă���
		for (int v = 0; v < polygonSize; v++)
		{
			int index{ pMesh_->GetPolygonVertex(poly, v) };
			pIndexData_[GetVertexCount(poly) + (polygonSize - 1 - v)] = static_cast<DWORD>(index);
		}
	}

	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(DWORD) * GetIndexCount(),
		.Usage = D3D11_USAGE_DEFAULT,          // �r���ŏ��������Ȃ�
		.BindFlags = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags = 0,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA
	{
		.pSysMem = pIndexData_,
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

void mtgb::Ground::InitializeConstantBuffer(ID3D11Device* _pDevice)
{
	const D3D11_BUFFER_DESC BUFFER_DESC
	{
		.ByteWidth = sizeof(ConstantBuffer),
		.Usage = D3D11_USAGE_DYNAMIC,  // MEMO: �r���ŏ��������邽��dynamic
		.BindFlags = D3D11_BIND_CONSTANT_BUFFER,
		.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};

	HRESULT hResult{};
	hResult = _pDevice->CreateBuffer(
		&BUFFER_DESC,
		nullptr,  // �����f�[�^�Ȃ�
		&pConstantBuffer_);

	massert(SUCCEEDED(hResult)
		&& "�R���X�^���g�o�b�t�@�̍쐬�Ɏ��s @FbxParts::InitializeConstantBuffer");
}
