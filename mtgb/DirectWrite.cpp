#include "DirectWrite.h"
#include "DirectX11Draw.h"
#include "MTAssert.h"
#include "Direct2D/Direct2D.h"
#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <d2d1.h>
#include <dwrite.h> 
#include <dwrite_1.h>
#include <dxgidebug.h>
#include "WindowContextUtil.h"
#include "Direct2DResource.h"
#include <clocale>
#include <cstdlib>
#pragma comment(lib,"dwrite.lib")

namespace
{
	std::string sampleText = "Hello,World!";
	const int DEFAULT_FONT_SIZE = 72;
	const wchar_t* fontFamilyName = L"Noto Sans JP";
}

ComPtr<IDWriteFactory> mtgb::DirectWrite::pDWriteFactory_{ nullptr };
ComPtr<IDWriteTextFormat> mtgb::DirectWrite::pTextFormat_{ nullptr };
//IDWriteTextLayout* mtgb::DirectWrite::pTextLayout_{ nullptr };
ComPtr<IDWriteFontCollection> mtgb::DirectWrite::pFontCollection_{ nullptr };
ComPtr<IDWriteFontFamily> mtgb::DirectWrite::pFontFamily_{nullptr};
ComPtr<IDWriteFont> mtgb::DirectWrite::pDWriteFont_{ nullptr };
DWRITE_FONT_METRICS mtgb::DirectWrite::fontMetrics_{};
mtgb::PixelFontMetrics mtgb::DirectWrite::pixelFontMetrics_;
//int mtgb::DirectWrite::currentDefaultFontSize_{ DEFAULT_FONT_SIZE };
//static std::wstring StrToWStr(const std::string& str);



mtgb::FontFormatData::FontFormatData(int size, IDWriteTextFormat* fmt, const PixelFontMetrics& metrics)
	: fontSize(size)
	, pixelFontMetrics(metrics)
{
	format.Attach(fmt);
}

mtgb::FontFormatData::~FontFormatData()
{
	format.Reset();
}

mtgb::TextLayoutData::TextLayoutData(const std::wstring& _str, int _size, float _width, float _height, IDWriteTextLayout* _layout, int _handle)
	: str(_str), fontSize(_size), width(_width), height(_height), handle(_handle)
{
	layout.Attach(_layout);
}

mtgb::TextLayoutData::~TextLayoutData()
{
	layout.Reset();
}

mtgb::DirectWrite::DirectWrite()
{
}

mtgb::DirectWrite::~DirectWrite()
{
}

void mtgb::DirectWrite::Initialize()
{
	HRESULT hResult = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		_uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(pDWriteFactory_.GetAddressOf())
	);
	massert(SUCCEEDED(hResult)
		&& "DWriteCreateFactory�Ɏ��s @DirectWrite::Initialize");

	
}

void mtgb::DirectWrite::CreateFontFormatData(const std::wstring& fileName, int fontSize, FontFormatData** ppFontFormatData)
{
	IDWriteFontCollection* fontCollection = nullptr;
	//�t�H���g�R���N�V�����擾
	HRESULT hResult = pDWriteFactory_->GetSystemFontCollection(&fontCollection);
	pFontCollection_.Attach(fontCollection);

	UINT32 index;
	BOOL exists;
	hResult = pFontCollection_->FindFamilyName(fileName.c_str(), &index, &exists);

	massert(SUCCEEDED(hResult)
		&& "FindFamilyName�Ɏ��s @DirectWrite::FindFamilyName");

	//�t�@�~���[����IDWriteFontFamily���擾
	IDWriteFontFamily* fontFamily = nullptr;
	hResult = pFontCollection_->GetFontFamily(index, &fontFamily);
	pFontFamily_.Attach(fontFamily);

	massert(SUCCEEDED(hResult)
		&& "GetFontFamily�Ɏ��s @DirectWrite::GetFontFamily");

	//IDWriteFont���擾
	IDWriteFont* font = nullptr;
	hResult = pFontFamily_->GetFirstMatchingFont(
		DWRITE_FONT_WEIGHT_REGULAR,
		DWRITE_FONT_STRETCH_NORMAL,//�t�H���g�̃X�g���b�`�B�g��k���̓x����
		DWRITE_FONT_STYLE_NORMAL,//�t�H���g�X�^�C��
		&font
	);
	pDWriteFont_.Attach(font);
	massert(SUCCEEDED(hResult)
		&& "GetFirstMatchingFont�Ɏ��s @DirectWrite::Initialize");

	//�t�H���g�̃��g���N�X�擾
	pDWriteFont_->GetMetrics(&fontMetrics_);

	// �V�����t�H�[�}�b�g���쐬
	IDWriteTextFormat* format = nullptr;
	PixelFontMetrics metrics;

	CreateTextFormat(fontSize, &format, metrics);

	delete *ppFontFormatData;
	//FontFormatData* data = new FontFormatData(fontSize, format, metrics);
	*ppFontFormatData = new FontFormatData(fontSize, format, metrics);

}

void mtgb::DirectWrite::Update()
{
}



void mtgb::DirectWrite::CreateTextFormat(int size, IDWriteTextFormat** ppTextFormat, PixelFontMetrics& outMetrics)
{
	//�e�L�X�g�t�H�[�}�b�g�쐬
	HRESULT hResult = pDWriteFactory_->CreateTextFormat(
		fontFamilyName,//�t�H���g�t�@�~���[�̖��O
		NULL,//�I���W�i���̃t�H���g���g���ꍇ�͂���p�̃I�u�W�F�N�g��n���BNULL�Ȃ�W�����ڂ̃t�H���g�t�@�~���[
		DWRITE_FONT_WEIGHT_REGULAR,//�t�H���g�̑���
		DWRITE_FONT_STYLE_NORMAL,//�t�H���g�X�^�C��
		DWRITE_FONT_STRETCH_NORMAL,//�t�H���g�̃X�g���b�`�B�g��k���̓x����
		static_cast<FLOAT>(size),//�t�H���g�̃T�C�Y
		L"en-us",//���P�[���Ben-us�̓A�����J�p��
		ppTextFormat//�쐬�����IDWriteTextFormat
	);

	massert(SUCCEEDED(hResult)
		&& "CreateTextFormat�Ɏ��s @DirectWrite::CreateTextFormat");

	//��������
	hResult = (*ppTextFormat)->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	massert(SUCCEEDED(hResult)
		&& "SetTextAlignment�Ɏ��s @DirectWrite::CreateTextFormat");

	//��������
	hResult = (*ppTextFormat)->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	massert(SUCCEEDED(hResult)
		&& "SetParagraphAlignment�Ɏ��s @DirectWrite::CreateTextFormat");

	//�f�U�C���P�ʂ���s�N�Z���P�ʂɕϊ�
	outMetrics.ascentPx = static_cast<float>(fontMetrics_.ascent) * static_cast<float>(size) / static_cast<float>(fontMetrics_.designUnitsPerEm);
	outMetrics.descentPx = static_cast<float>(fontMetrics_.descent) * static_cast<float>(size) / static_cast<float>(fontMetrics_.designUnitsPerEm);
	outMetrics.lineGapPx = static_cast<float>(fontMetrics_.lineGap) * static_cast<float>(size) / static_cast<float>(fontMetrics_.designUnitsPerEm);
	//�e�L�X�g�̏�[���w����W�ɂقڂ҂����葵���邽�߂̃I�t�Z�b�g
	outMetrics.textTopOffset = -outMetrics.ascentPx + (outMetrics.ascentPx - outMetrics.descentPx) - outMetrics.lineGapPx;
}

void mtgb::DirectWrite::SetTextAlignment(TextAlignment alignment, ComPtr<IDWriteTextFormat>format)
{
	switch (alignment)
	{
	case TextAlignment::topLeft:
		format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	case TextAlignment::center:
		format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	case TextAlignment::bottomLeft:
		format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
		break;
	case TextAlignment::middleLeft:
		format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		break;
	}
}

void mtgb::DirectWrite::CreateTextLayout(const std::wstring& _str, int  _size, ComPtr<IDWriteTextFormat> _format, IDWriteTextLayout** _ppTextLayout)
{
	//�e�L�X�g���C�A�E�g�쐬
	D2D1_SIZE_F rtSize = Game::System<Direct2D>().pDefRenderTarget_->GetSize();
	CreateTextLayout(_str, rtSize.width, rtSize.height, _size, _format, _ppTextLayout);
}

void mtgb::DirectWrite::CreateTextLayout(const std::wstring& _str, float  _width, float _height, int  _size, ComPtr<IDWriteTextFormat> _format, IDWriteTextLayout** _ppTextLayout)
{
	static const float dip = 96.0f;
	FLOAT dpiX, dpiY;
	Game::System<Direct2D>().pDefRenderTarget_->GetDpi(&dpiX, &dpiY);

	_width = _width / dpiX * dip;
	_height = _height / dpiY * dip;

	HRESULT hResult = pDWriteFactory_->CreateTextLayout(_str.data(), static_cast<uint32_t>(_str.size()), _format.Get(), _width, _height, _ppTextLayout);

	massert(SUCCEEDED(hResult)
		&& "CreateTextLayout�Ɏ��s @DirectWrite::RegisterText");
}

void mtgb::DirectWrite::ChangeFormat(ComPtr<IDWriteTextFormat> format, mtgb::PixelFontMetrics& metrics)
{
	pTextFormat_ = format;
	pixelFontMetrics_ = metrics;
}

void mtgb::DirectWrite::Draw(ComPtr<IDWriteTextLayout> textLayout, float x, float y)
{
	D2D1_POINT_2F origin = { x, y };

	Game::System<Direct2D>().pDefRenderTarget_->BeginDraw();
	Game::System<Direct2D>().pDefRenderTarget_->DrawTextLayout(origin, textLayout.Get(), Game::System<Direct2D>().pDefD2DBrush_.Get());
	Game::System<Direct2D>().pDefRenderTarget_->EndDraw();
}

void mtgb::DirectWrite::ImmediateDraw(const std::wstring& text, float x, float y)
{
	ImmediateDraw(text, pTextFormat_, pixelFontMetrics_, x, y);
}

void mtgb::DirectWrite::ImmediateDraw(const std::wstring& text, ComPtr<IDWriteTextFormat> format, const PixelFontMetrics& pixelFontMetrics, float x, float y, float width, float height)
{
	Game::System<Direct2D>().pDefRenderTarget_->BeginDraw();

	Game::System<Direct2D>().pDefRenderTarget_->DrawText(
		text.c_str(),
		static_cast<uint32_t>(text.length()),
		format.Get(),
		D2D1::RectF(
			x,
			y + pixelFontMetrics.textTopOffset,
			x + width,
			y + pixelFontMetrics.textTopOffset + height),
		Game::System<Direct2D>().pDefD2DBrush_.Get()
	);

	Game::System<Direct2D>().pDefRenderTarget_->EndDraw();
}

void mtgb::DirectWrite::ImmediateDraw(const std::wstring& text, ComPtr<IDWriteTextFormat> format, const PixelFontMetrics& pixelFontMetrics, float x, float y)
{
	D2D1_SIZE_F rtSize = Game::System<Direct2D>().pDefRenderTarget_->GetSize();

	ImmediateDraw(text, format, pixelFontMetrics, x, y, rtSize.width, rtSize.height);
}



void mtgb::DirectWrite::Release()
{
	pDWriteFont_.Reset();
	pFontFamily_.Reset();
	pFontCollection_.Reset();
	pTextFormat_.Reset();
	pDWriteFactory_.Reset();

	
}

//static std::wstring StrToWStr(const std::string& str)
//{
//	//nullptr���w�肷��ƌ��݂̃��P�[�������擾�ł���
//	std::string oldLocale = setlocale(LC_CTYPE, nullptr);
//
//	setlocale(LC_CTYPE, "Japanese_Japan.932");
//
//	size_t len;
//	//�܂��}���`�o�C�g������(str)�����C�h������ɕϊ������Ƃ��̒������擾
//	mbstowcs_s(&len, nullptr, 0, str.c_str(), _TRUNCATE);
//	//���C�h������̃o�b�t�@�p��
//	std::wstring wText;
//	wText.resize(len);
//
//	//���ۂɃ��C�h������ɕϊ�
//	//_TRUNCATE�萔�ŁA�o�b�t�@�Ɏ��܂�Ȃ������͐؂�̂Ă�悤�w�肷��
//	mbstowcs_s(&len, wText.data(), len, str.c_str(), _TRUNCATE);
//
//	setlocale(LC_CTYPE, oldLocale.c_str());
//	return wText;
//}
