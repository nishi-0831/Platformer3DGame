#include "Sprite.h"
#include "Transform.h"
#include "Debug.h"


mtgb::Sprite::Sprite() :
	texture2D_{}
{
}

mtgb::Sprite::~Sprite()
{
}

void mtgb::Sprite::Load(const std::wstring& _fileName)
{
	fileName_ = _fileName;
	texture2D_.Load({ _fileName });
}

void mtgb::Sprite::Draw(
	const RectF& _draw,
	const float _rotationZ,
	const RectF& _cut,
	const Color& _color)
{
	using DirectX::XMMatrixScaling;      // �g�k
	using DirectX::XMMatrixTranspose;    // �s�Ɨ�����ւ���
	using DirectX::XMMatrixTranslation;  // ���s�ړ�
	using DirectX::XMMatrixIdentity;     // �P�ʍs��
	using DirectX::XMMatrixRotationZ;    // Z���̉�]�s��

	DirectX11Draw::SetBlendMode(BlendMode::Sprite);  // �u�����h���[�h�f�t�H���g
	DirectX11Draw::SetIsWriteToDepthBuffer(false);    // �[�x�o�b�t�@�ւ̏������݂Ȃ�

	IShader::Draw<ConstantBuffer, Vertex>(
		// �V�F�[�_�ɓn�����
		[&, this](ConstantBuffer* _pCB)
		{
			_pCB->g_color = _color.ToVector4Norm();
			_pCB->g_angle = {};
			_pCB->g_matrixCameraRotation = XMMatrixIdentity();  // �J�����͖��� = UI���W
			_pCB->g_matrixCameraTranslate = XMMatrixIdentity();
			_pCB->g_matrixWorldRotation = XMMatrixRotationZ(_rotationZ);

#pragma region TODO: �v�Z�������K�v
			// �X�N���[���T�C�Y���擾
			const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

			// ���w���W�ƕ`����W��y�����ى���
			RectF cartesianBox{ _draw };
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

			_pCB->g_matrixWorldTranslate = XMMatrixTranspose(world);
#pragma endregion

#pragma region UV�v�Z
			// �g���~���O�v�Z

			const Vector2F CUT_BEGIN{ _cut.GetBegin() };
			const Vector2F CUT_END{ _cut.GetEnd() };

			// �g���~���O��`�̍���_����s�ړ�
			Matrix4x4 uvMove = XMMatrixTranslation(
				CUT_BEGIN.x * 1.0f / texture2D_.GetSize().x,
				CUT_BEGIN.y * 1.0f / texture2D_.GetSize().y,
				0.0f);

			// �g���~���O��`�̊g�k
			Matrix4x4 uvScaling = XMMatrixScaling(
				static_cast<float>(CUT_END.x) / texture2D_.GetSize().x,
				static_cast<float>(CUT_END.y) / texture2D_.GetSize().y,
				1.0f);

			Matrix4x4 uv{ uvScaling * uvMove };

			_pCB->g_matrixTexture = XMMatrixTranspose(uv);
#pragma endregion
		},
		[&, this](ID3D11DeviceContext* _pDC)
		{
			ID3D11SamplerState* pSamplerState{ texture2D_.GetSamplerState() };
			_pDC->PSSetSamplers(0, 1, &pSamplerState);

			ID3D11ShaderResourceView* pSRV{ texture2D_.GetShaderResourceView() };
			_pDC->PSSetShaderResources(0, 1, &pSRV);
		});

#if 0  // �e�X�g�p (GPU�ɑ������f�[�^���Q�Ƃ���)
	{  // ���_�o�b�t
		// 1. �X�e�[�W���O�o�b�t�@�̍쐬
		ID3D11Buffer* pStagingBuffer = nullptr;
		D3D11_BUFFER_DESC stagingDesc;
		ZeroMemory(&stagingDesc, sizeof(stagingDesc));
		stagingDesc.ByteWidth = sizeof(Vertex) * 4;
		stagingDesc.Usage = D3D11_USAGE_STAGING;
		stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingDesc.StructureByteStride = 0;

		DirectX11Draw::pDevice_->CreateBuffer(&stagingDesc, nullptr, &pStagingBuffer);

		// 2. GPU�o�b�t�@����R�s�[
		DirectX11Draw::pContext_->CopyResource(pStagingBuffer, pVertexBuffer_);

		// 3. �f�[�^�}�b�s���O
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(DirectX11Draw::pContext_->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
		{
			Vertex retrievedData[4];
			memcpy(&retrievedData, mappedResource.pData, sizeof(Vertex) * 4);

			DirectX11Draw::pContext_->Unmap(pStagingBuffer, 0);
		}

		// 4. ���\�[�X���
		pStagingBuffer->Release();
	}

	{  // �R���X�^���g�o�b�t
		// 1. �X�e�[�W���O�o�b�t�@�̍쐬
		ID3D11Buffer* pStagingBuffer = nullptr;
		D3D11_BUFFER_DESC stagingDesc;
		ZeroMemory(&stagingDesc, sizeof(stagingDesc));
		stagingDesc.ByteWidth = sizeof(ConstantBuffer);
		stagingDesc.Usage = D3D11_USAGE_STAGING;
		stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stagingDesc.StructureByteStride = 0;

		DirectX11Draw::pDevice_->CreateBuffer(&stagingDesc, nullptr, &pStagingBuffer);

		// 2. GPU�o�b�t�@����R�s�[
		DirectX11Draw::pContext_->CopyResource(pStagingBuffer, pConstantBuffer_);

		// 3. �f�[�^�}�b�s���O
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (SUCCEEDED(DirectX11Draw::pContext_->Map(pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
		{
			ConstantBuffer retrievedData;
			memcpy(&retrievedData, mappedResource.pData, sizeof(ConstantBuffer));

			DirectX11Draw::pContext_->Unmap(pStagingBuffer, 0);
		}

		// 4. ���\�[�X���
		pStagingBuffer->Release();
	}
#endif
}

void mtgb::Sprite::Draw(const Transform* _pTransform, const Transform* _pCameraTransform, const Vector2Int& _imageSize, const Color& _color)
{
	massert(_pTransform != nullptr
		&& "�`�悷��_pTransform��nullptr���w�肳�ꂽ @Sprite::Draw");

	using DirectX::XMMatrixScaling;             // �g�k
	using DirectX::XMMatrixTranspose;           // �s�Ɨ�����ւ���
	using DirectX::XMMatrixTranslation;         // ���s�ړ�
	using DirectX::XMMatrixIdentity;            // �P�ʍs��
	using DirectX::XMMatrixRotationQuaternion;  // ��]�s��

	DirectX11Draw::SetBlendMode(BlendMode::Default);  // �u�����h���[�h�f�t�H���g
	DirectX11Draw::SetIsWriteToDepthBuffer(false);    // �[�x�o�b�t�@�ւ̏������݂Ȃ�

	IShader::Draw<ConstantBuffer, Vertex>(
		// �V�F�[�_�ɓn�����
		[&, this](ConstantBuffer* _pCB)
		{
			_pCB->g_color = _color.ToVector4Norm();
			_pCB->g_angle = {};

			_pCB->g_matrixCameraTranslate = XMMatrixIdentity();
			if (_pCameraTransform != nullptr)
			{
				// �J����������Ȃ�J�����̃��[���h�s��𐶐����ē���Ă���
				_pCameraTransform->GenerateWorldMatrix(&_pCB->g_matrixCameraTranslate);
				_pCB->g_matrixCameraTranslate = XMMatrixTranspose(_pCB->g_matrixCameraTranslate);

				_pCameraTransform->GenerateWorldRotationMatrix(&_pCB->g_matrixCameraRotation);
				_pCB->g_matrixCameraTranslate = XMMatrixTranspose(_pCB->g_matrixCameraRotation);
			}
			Matrix4x4 test = _pCB->g_matrixCameraTranslate;


#pragma region TODO: �v�Z�������K�v
			// �X�N���[���T�C�Y����x�����擾
			static const Vector2Int SCREEN_SIZE{ Game::System<Screen>().GetSize() };

			// ���w���W�ƕ`����W��y�����ى���
			/*RectInt cartesianBox{ _draw };
			cartesianBox.y = SCREEN_SIZE.y - cartesianBox.y;
			cartesianBox.height *= -1;*/

			/*const Vector2Int VIEW_BEGIN{ cartesianBox.GetBegin() };
			const Vector2Int VIEW_END{ cartesianBox.GetEnd() };*/

			// �\������T�C�Y�ɍ��킹��
			/*Matrix4x4 scalingBox = XMMatrixScaling(
				std::abs(VIEW_END.x - VIEW_BEGIN.x) * 2.0f,
				std::abs(VIEW_END.y - VIEW_BEGIN.y) * 2.0f,
				1.0f);*/

			// �\������{�b�N�X�̈ʒu���ړ�����
			//DirectX::XMMatrixTranslationFromVector(_pTransform->position_);
			
			// �{�b�N�X�̍��W�ϊ�
			Matrix4x4 boxTranslate{}; 
			_pTransform->GenerateParentMatrix(&boxTranslate);

			//// �摜�T�C�Y�ɍ��킹��
			Matrix4x4 scalingBox = XMMatrixScaling(
				static_cast<float>(_imageSize.x),
				static_cast<float>(_imageSize.y),
				1.0f);

			// �I�t�Z�b�g - ��ʒ��S��(0, 0) ������(-1, -1)
			/*Matrix4x4 offsetView
			{
				XMMatrixTranslation(-1.0f, -1.0f, 0.0f)
			};*/

			// ��ʂɍ��킹��
			Matrix4x4 scalingView = XMMatrixScaling(
				1.0f / (SCREEN_SIZE.x * 1),
				1.0f / (SCREEN_SIZE.y * 1),
				1.0f);

			// �ŏI�I�ȍs��
			Matrix4x4 worldTranslate
			{
				scalingBox * scalingView * boxTranslate
			};
			_pCB->g_matrixWorldTranslate = XMMatrixTranspose(worldTranslate);

			_pTransform->GenerateWorldRotationMatrix(&_pCB->g_matrixWorldRotation);
			_pCB->g_matrixWorldRotation = XMMatrixTranspose(_pCB->g_matrixWorldRotation);
#pragma endregion

#pragma region UV�v�Z
			_pCB->g_matrixTexture = XMMatrixIdentity();
#pragma endregion
		},
		[&, this](ID3D11DeviceContext* _pDC)
		{
			ID3D11SamplerState* pSamplerState{ texture2D_.GetSamplerState() };
			_pDC->PSSetSamplers(0, 1, &pSamplerState);

			ID3D11ShaderResourceView* pSRV{ texture2D_.GetShaderResourceView() };
			_pDC->PSSetShaderResources(0, 1, &pSRV);
		});
}

void mtgb::Sprite::InitializeVertexBuffer(ID3D11Device* _pDevice)
{
	Vertex vertexes[]
	{
		{ Vector3{ -1,  1,  0 }, Vector3{ 0, 0, 0 } },  // ����
		{ Vector3{  1,  1,  0 }, Vector3{ 1, 0, 0 } },  // �E��
		{ Vector3{ -1, -1,  0 }, Vector3{ 0, 1, 0 } },  // ����
		{ Vector3{  1, -1,  0 }, Vector3{ 1, 1, 0 } },  // �E��
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
		pVertexBuffer_.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // ���_�o�b�t�@�̍쐬�ɐ���
		&& "���_�o�b�t�@�̍쐬�Ɏ��s @Sprite::InitializeVertexBuffer");
}

void mtgb::Sprite::InitializeIndexBuffer(ID3D11Device* _pDevice)
{
	static const int INDEXES[]{ 2, 1, 0, 2, 3, 1 };
	//static const int INDEXES[]{ 0, 1, 2, 2, 1, 3 };

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
		pIndexBuffer_.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // �C���f�b�N�X�o�b�t�@�̍쐬�ɐ���
		&& "�C���f�b�N�X�o�b�t�@�̍쐬�Ɏ��s @Sprite::InitializeIndexBuffer");
}

void mtgb::Sprite::InitializeConstantBuffer(ID3D11Device* _pDevice)
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
		pConstantBuffer_.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)
		&& "�R���X�^���g�o�b�t�@�̍쐬�Ɏ��s @Sprite::InitializeConstantBuffer");
}
