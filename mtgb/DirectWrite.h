#pragma once
#include <cmtgb.h>
#include <string>
#include <functional>
#include <unordered_map>
#include <tuple>
#include <cmath>

#include "RectInt.h"
#include "ReleaseUtility.h"
#include "TextAlignment.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct IDWriteFactory;
struct IDWriteTextFormat;
struct IDWriteTextLayout;
struct IDWriteFontCollection;
struct IDWriteFontFamily;
struct IDWriteFont;
struct DWRITE_FONT_METRICS;
struct ID2D1Brush;

namespace mtgb
{
	//using TextLayoutKey = std::tuple<std::wstring, int>; // ������ƃT�C�Y�̃y�A
	
	struct PixelFontMetrics
	{
		float ascentPx;
		float descentPx;
		float lineGapPx;
		float textTopOffset;
	};

	// �t�H���g�t�H�[�}�b�g�̃L���b�V���G���g��
	struct FontFormatData
	{
		int fontSize;
		ComPtr<IDWriteTextFormat> format;
		PixelFontMetrics pixelFontMetrics;
		
		FontFormatData(int size, IDWriteTextFormat* fmt, const PixelFontMetrics& metrics);
		
		~FontFormatData();
	};

	// �e�L�X�g���C�A�E�g�̃L���b�V���G���g��
	struct TextLayoutData
	{
		std::wstring str;
		int fontSize;
		float width, height;

		ComPtr<IDWriteTextLayout> layout;
		int handle;
		
		TextLayoutData(const std::wstring& _str, int _size, float _width, float _height, IDWriteTextLayout* _layout, int _handle);
		
		~TextLayoutData();
	};

	

	/// <summary>
	/// DirectWrite�p�̃V�X�e��
	/// </summary>
	class DirectWrite : public ISystem
	{
	public:
		DirectWrite();
		~DirectWrite();

		/// <summary>
		/// ����������
		/// </summary>
		void Initialize() override;


		void CreateFontFormatData(const std::wstring& fileName,int fontSize, FontFormatData** ppFontFormatData);
		/// <summary>
		/// �X�V����
		/// </summary>
		void Update() override;

		/// <summary>
		/// �n���h���ɉ������e�L�X�g��`��
		/// ���S�ɓ����������`�悵������ꍇ�ɓK���Ă���
		/// </summary>
		/// <param name="handle"></param>
		/// <param name="x">�e�L�X�g�̍��[</param>
		/// <param name="y">�e�L�X�g�̏�[</param>
		//void Draw(int handle, float x, float y);

		void Draw(ComPtr<IDWriteTextLayout> textLayout, float x, float y);


		/// <summary>
		/// �����`��
		/// �����񂾂����p�ɂɕω�����ꍇ�ɓK���Ă���
		/// </summary>
		/// <param name="text">�`�悷��e�L�X�g</param>
		/// <param name="x">�e�L�X�g�̍��[</param>
		/// <param name="y">�e�L�X�g�̏�[</param>
		void ImmediateDraw(const std::wstring& text, float x, float y);
		

		
		//void ImmediateDraw(const std::string& text, float x, float y, int size);
		void ImmediateDraw(const std::wstring& text, ComPtr<IDWriteTextFormat> format, const PixelFontMetrics& pixelFontMetrics, float x,float y,float width,float height);

		void ImmediateDraw(const std::wstring& text, ComPtr<IDWriteTextFormat> format, const PixelFontMetrics& pixelFontMetrics, float x, float y);
		
		/// <summary>
		/// �f�t�H���g�t�H���g�T�C�Y��ύX
		/// </summary>
		/// <param name="size">�V�����f�t�H���g�T�C�Y</param>
		//void ChangeFontSize(int size);

		void ChangeFormat(ComPtr<IDWriteTextFormat> format, mtgb::PixelFontMetrics& metrics);

		void Release() override;

		

		
		
		/// <summary>
		/// IDWriteTextLayout���쐬
		/// </summary>
		/// <param name="str">������</param>
		/// <param name="size">�傫��</param>
		/// <param name="format"></param>
		/// <param name="ppTextLayout">�쐬�����IDWriteTextLayout</param>
		void CreateTextLayout(const std::wstring & _str, int  _size, ComPtr<IDWriteTextFormat> _format, IDWriteTextLayout ** _ppTextLayout);
		void CreateTextLayout(const std::wstring & _str, float  _width, float _height, int  _size, ComPtr<IDWriteTextFormat> _format, IDWriteTextLayout ** _ppTextLayout);

		

		/// <summary>
		/// IDWriteTextFormat���쐬
		/// </summary>
		/// <param name="size">�t�H���g�T�C�Y</param>
		/// <param name="ppTextFormat">�쐬�����IDWriteTextFormat</param>
		/// <param name="outMetrics">�v�Z�����PixelFontMetrics</param>
		void CreateTextFormat(int size, IDWriteTextFormat** ppTextFormat, PixelFontMetrics& outMetrics);

		void SetTextAlignment(TextAlignment alignment, ComPtr<IDWriteTextFormat> format);
	private:
	
		static DWRITE_FONT_METRICS fontMetrics_;
		static PixelFontMetrics pixelFontMetrics_;
		static ComPtr<IDWriteFactory> pDWriteFactory_;
		static ComPtr<IDWriteTextFormat> pTextFormat_; // �f�t�H���g�t�H�[�}�b�g
		//static IDWriteTextLayout* pTextLayout_;
		static ComPtr<IDWriteFontCollection> pFontCollection_;
		static ComPtr<IDWriteFontFamily> pFontFamily_;
		static ComPtr<IDWriteFont> pDWriteFont_;
		
		//static int currentDefaultFontSize_; // ���݂̃f�t�H���g�t�H���g�T�C�Y
	};
}
