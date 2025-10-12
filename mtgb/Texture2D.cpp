#include "Texture2D.h"
#include <d3d11.h>
#include <wincodec.h>
#include "MTAssert.h"
#include "ReleaseUtility.h"
#include "DirectX11Draw.h"

mtgb::Texture2D::Texture2D() :
	pSamplerLinear_{ nullptr },
	pShaderResourceView_{ nullptr },
	size_{ Vector2Int::Zero() }
{
}

mtgb::Texture2D::~Texture2D()
{
	pSamplerLinear_.Reset();
	pShaderResourceView_.Reset();
}

void mtgb::Texture2D::Load(const std::wstring& _fileName)
{
	HRESULT hResult{};

	// TODO: COM�𐳂�����������
	// MEMO: MTA�ɑ�����悤�ɂ���
	hResult = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	massert(SUCCEEDED(hResult)  // CoInitializeEx�ɐ������Ă���
		&& "CoInitializeEx�ł��Ȃ����� @Texture2D::Load");

	IWICImagingFactory* pFactory{ nullptr };
	IWICBitmapDecoder* pDecoder{ nullptr };
	IWICBitmapFrameDecode* pFrame{ nullptr };
	IWICFormatConverter* pFormatConverter{ nullptr };

	hResult = CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		reinterpret_cast<void**>(&pFactory));

	massert(SUCCEEDED(hResult)  // IWICImagingFactory�̍쐬�ɐ���
		&& "CoCreateInstance�Ɏ��s @Texture2D::Load");

	// CreateDecoderFromFilename�őΉ����Ă��� wchar �֕ϊ�
	//std::wstring fileNameWStr{ _fileName.begin(), _fileName.end() };

	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/wincodec/nf-wincodec-iwicimagingfactory-createdecoderfromfilename
	//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/wincodec/ne-wincodec-wicdecodeoptions
	hResult = pFactory->CreateDecoderFromFilename(
		_fileName.c_str(),  // �I�u�W�F�N�g�̃t�@�C����
		nullptr,  // �D�悵�����x���_: �w�薳��
		GENERIC_READ,  // �I�u�W�F�N�g�֓ǂݎ��A�N�Z�X
		WICDecodeMetadataCacheOnDemand,  // �K�v�ɉ����ă��^�f�[�^���L���b�V������
		&pDecoder);  // �f�R�[�_��n��

	massert(SUCCEEDED(hResult)  // �t�@�C���̃f�R�[�h�ɐ���
		&& "pFactory->CreateDecoderFromFilename�Ɏ��s @Texture2D::Load");

	// 0�ԃt���[�����擾
	// MEMO: GIF�̂悤�ȕ����t���[������ꍇ��index��������w��ł���
	hResult = pDecoder->GetFrame(0, &pFrame);

	massert(SUCCEEDED(hResult)  // 0�ԃt���[�����擾�ł��Ă���
		&& "0�ԃt���[���̎擾�Ɏ��s @Texture2D::Load");

	hResult = pFactory->CreateFormatConverter(&pFormatConverter);

	massert(SUCCEEDED(hResult)  // �t�H�[�}�b�g�R���o�[�^�̍쐬�ɐ���
		&& "pFactory->CreateFormatConverter�Ɏ��s @Texture2D::Load");

	// TODO: �ϊ��̃f�B�U�𗝉�����
	hResult = pFormatConverter->Initialize(
		pFrame,  // �ϊ����������
		GUID_WICPixelFormat32bppRGBA,  // �ϊ���̃s�N�Z���`��
		WICBitmapDitherTypeNone,  // �g�p�f�B�U(?)�A���S���Y��
		nullptr,  // �g�p�p���b�g
		1.0,  // �g�p�A�t�@臒l
		WICBitmapPaletteTypeMedianCut);  // �����l�J�b�g�A���S���Y��(?)

	massert(SUCCEEDED(hResult)  // �t�H�[�}�b�g�R���o�[�^�̏������ɐ������Ă���
		&& "�t�H�[�}�b�g�R���o�[�^�̏������Ɏ��s @Texture2D::Load");

	UINT imageWidth{};   // �摜�̉���
	UINT imageHeight{};  // �摜�̏c��
	hResult = pFormatConverter->GetSize(&imageWidth, &imageHeight);

	massert(SUCCEEDED(hResult)  // �摜�T�C�Y�̎擾�ɐ������Ă���
		&& "�t�H�[�}�b�g�R���o�[�^����摜�T�C�Y�̎擾�Ɏ��s @Texture2D::Load");

	// �摜�̃T�C�Y��Vector2Int�ɕϊ����ă����o�ϐ��ɓ���Ă���
	size_ = Vector2Int{ static_cast<int>(imageWidth), static_cast<int>(imageHeight) };

	// �s�N�Z���f�[�^���ꎞ�I�ɕۑ�����o�b�t�@��p��
	std::vector<BYTE> pixelData(imageWidth * imageHeight * 4);
	UINT stride = imageWidth * 4;

	hResult = pFormatConverter->CopyPixels(
		nullptr,
		stride,
		static_cast<UINT>(pixelData.size()),
		pixelData.data());

	/*hResult = pFormatConverter->CopyPixels(
		nullptr,
		hMappedSubresource.RowPitch,
		imageHeight * hMappedSubresource.RowPitch,
		static_cast<BYTE*>(hMappedSubresource.pData));*/

	massert(SUCCEEDED(hResult)
		&& "�e�N�X�`���̃s�N�Z���R�s�[�Ɏ��s @Texture2D::Load");

	// �~�b�v�}�b�v���x�����v�Z
	int mipLevels = static_cast<int>(std::floor(std::log2((std::max)(imageWidth, imageHeight)))) + 1;

	ID3D11Texture2D* pTexture{ nullptr };

	const D3D11_TEXTURE2D_DESC TEXTURE2D_DESC  // �e�N�X�`���̐ݒ�
	{
		.Width = static_cast<UINT>(size_.x),
		.Height = static_cast<UINT>(size_.y),
		.MipLevels = static_cast<UINT>(mipLevels),
		.ArraySize = 1,
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.SampleDesc
		{
			.Count = 1,
			.Quality = 0
		},
		.Usage = D3D11_USAGE_DEFAULT,
		//.Usage = D3D11_USAGE_DYNAMIC,
		.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET,
		//.BindFlags = D3D11_BIND_SHADER_RESOURCE,
		.CPUAccessFlags = 0,
		//.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS,
		//.MiscFlags = 0,
	};

	// 2D�e�N�X�`�����쐬����
	hResult = DirectX11Draw::pDevice_->CreateTexture2D(
		&TEXTURE2D_DESC,  // �e�N�X�`���̐ݒ�
		nullptr,         // �T�u���\�[�X�̃|�C���^
		&pTexture);      // �쐬�����e�N�X�`���̃|�C���^�n��

	massert(SUCCEEDED(hResult)  //2D�e�N�X�`���̍쐬�ɐ���
		&& "2D�e�N�X�`���̍쐬�Ɏ��s @Texture2D::Load");

	D3D11_MAPPED_SUBRESOURCE hMappedSubresource{};

	//// �e�N�X�`�����}�b�v(�o�^)����
	//hResult = DirectX11Draw::pContext_->Map(
	//	pTexture,
	//	0U,
	//	D3D11_MAP_WRITE_DISCARD,
	//	0U,
	//	&hMappedSubresource);

	//massert(SUCCEEDED(hResult)  // �e�N�X�`���̃}�b�v�ɐ���
	//	&& "�e�N�X�`���̃}�b�v�Ɏ��s @Texture2D::Load");

	

	/*DirectX11Draw::pContext_->Unmap(pTexture, 0U);*/

	const D3D11_SHADER_RESOURCE_VIEW_DESC SHADER_RESOURCE_VIEW_DESC
	{
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D,
		.Texture2D
		{
			.MostDetailedMip = 0U,
			.MipLevels = static_cast<UINT>(mipLevels),
		}
	};

	hResult = DirectX11Draw::pDevice_->CreateShaderResourceView(
		pTexture,
		&SHADER_RESOURCE_VIEW_DESC,
		pShaderResourceView_.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)  // �e�N�X�`���p�V�F�[�_���\�[�X�r���[�̍쐬�ɐ���
		&& "�e�N�X�`���p�V�F�[�_���\�[�X�r���[�̍쐬�Ɏ��s  @Texture2D::Load");

	// CPU���̃f�[�^��GPU�֏�������
	DirectX11Draw::pContext_->UpdateSubresource(
		pTexture,           // ���\�[�X
		0,                  // �T�u���\�[�X�C���f�b�N�X�i�ō��𑜓x�~�b�v�j
		nullptr,            // �X�V�͈́i�S�́j
		pixelData.data(),   // �s�N�Z���f�[�^
		stride,             // �s�s�b�`
		0);
	
	// �~�b�v�}�b�v�𐶐�
	DirectX11Draw::pContext_->GenerateMips(pShaderResourceView_.Get());

	// �T���v���[�X�e�[�g�̐ݒ�(�~�b�v�}�b�v�Ή���)
	const D3D11_SAMPLER_DESC SAMPLER_DESC  
	{
		// MEMO: �T���v�����O����Ƃ��̃t�B���^ (���Ȃ��ޑ���)
		//     : Unity�̃e�N�X�`���ݒ�݂����Ȋ���
		//  REF: https://learn.microsoft.com/ja-jp/windows/win32/api/d3d11/ne-d3d11-d3d11_filter
		//.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT,
		.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		.AddressU = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressV = D3D11_TEXTURE_ADDRESS_WRAP,
		.AddressW = D3D11_TEXTURE_ADDRESS_WRAP,
		.MipLODBias = 0.0f,  // �~�b�v�}�b�v��LOD�I�t�Z�b�g
		.MaxAnisotropy = 0U,  // �ٕ����t�B���^���g�����ꍇ�̃N�����v�l
		.ComparisonFunc = D3D11_COMPARISON_ALWAYS,  // TOOD: D3D11_COMPARISON_FUNC�ɂ��ė�������
		.BorderColor = {},  // �{�[�_�̐F
		.MinLOD = {},  // LOD�~�b�v�}�b�v�̍ŏ����x��
		.MaxLOD = D3D11_FLOAT32_MAX,  // LOD�~�b�v�}�b�v�̍ő僌�x��
	};

	// �T���v���[�X�e�[�g���쐬
	hResult = DirectX11Draw::pDevice_->CreateSamplerState(&SAMPLER_DESC, pSamplerLinear_.ReleaseAndGetAddressOf());

	massert(SUCCEEDED(hResult)
		&& "�T���v���[�X�e�[�g�̍쐬�Ɏ��s @Texture2D::Load");

	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pFormatConverter);
	SAFE_RELEASE(pFrame);
	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pFactory);

	CoUninitialize();
}
