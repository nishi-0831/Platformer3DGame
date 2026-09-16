#include "TextCache.h"
#include "Core/Game.h"
#include "Graphics/DirectX11Draw.h"
#include "DirectWrite.h"
#include "Utility/MTStringUtility.h"
#include <dwrite.h>
#include "Graphics/Direct2D.h"
const wchar_t* mtgb::TextCache::DEFAULT_FONT_FAMILY_NAME { L"Noto Sans JP" };

namespace
{
}
mtgb::TextCache::TextCache()
	: currentDefaultFontSize_ { 72 }
	, nextHandle_ { 0 }
{
	textLayoutData_ = new TextLayoutContainer();
	fontFormatData_ = new FontFormatContainer();
}

mtgb::TextCache::~TextCache() {}

void mtgb::TextCache::Initialize() {}

void mtgb::TextCache::Release()
{
	if (textLayoutData_ != nullptr)
	{
		for (auto entry : *textLayoutData_)
		{
			SAFE_DELETE(entry);
		}
		textLayoutData_->clear();
		SAFE_DELETE(textLayoutData_);
	}

	if (fontFormatData_ != nullptr)
	{
		for (auto entry : *fontFormatData_)
		{
			SAFE_DELETE(entry);
		}
		fontFormatData_->clear();
		SAFE_DELETE(fontFormatData_);
	}
}

void mtgb::TextCache::Update() {}

int mtgb::TextCache::Load(const std::string& _str, int _size)
{
	return Game::System<TextCache>().GetOrCreateTextLayoutHandle(MultiToWide(_str), _size);
}

int mtgb::TextCache::Load(const std::string& _str, int _fontSize, float _layoutBoxWidth, float _layoutBoxHeight)
{
	return Game::System<TextCache>()
		.GetOrCreateTextLayoutHandle(MultiToWide(_str), _fontSize, _layoutBoxWidth, _layoutBoxHeight);
}

int mtgb::TextCache::Load(const std::string& _str, int _fontSize, Vector2F _layoutBoxSize)
{
	return Game::System<TextCache>()
		.GetOrCreateTextLayoutHandle(MultiToWide(_str), _fontSize, _layoutBoxSize.x, _layoutBoxSize.y);
}

#if false
void mtgb::Text::Draw(int handle, float x, float y)
{
	DirectX11Draw::SetIsWriteToDepthBuffer(false);
	auto& handle_index = textLayoutDatas_->get<handle_order>();
	auto it = handle_index.find(handle);

	if (it == handle_index.end()) return;

	const auto& entry = *it;

	// そのテキストレイアウトに対応するフォントサイズのメトリクスを取得
	auto formatData = GetOrCreateTextFormat(entry->fontSize);
	PixelFontMetrics metrics = formatData.second;
	
	Game::System<DirectWrite>().SetTextAlignment(currentTextAlignment_, entry->layout);
	Game::System<DirectWrite>().Draw(entry->layout, x, y + metrics.textTopOffset);
}



void mtgb::Text::ImmediateDraw(const std::wstring& text, float x, float y, int size)
{
	D2D1_SIZE_F layoutBox = Game::System<Direct2D>().GetRenderTargetSize();
}

void mtgb::Text::ImmediateDraw(const std::string& text, float x, float y, int size)
{
	ImmediateDraw(ToWString(text), x, y, size);
}

void mtgb::Text::ImmediateDraw(const std::string& text, RectInt rect, int size)
{
}

void mtgb::Text::ImmediateDraw(const std::string& text, float x, float y)
{
	ImmediateDraw(text, x, y, currentDefaultFontSize_);
}

void mtgb::Text::ImmediateDraw(const std::wstring& text, float x, float y)
{
	ImmediateDraw(text, x, y, currentDefaultFontSize_);

}

void mtgb::Text::ImmediateDraw(const std::wstring& text, float x, float y, float width, float height, int size)
{
	// 指定サイズのフォーマットを取得または作成
	auto formatData = GetOrCreateTextFormat(size);

	Game::System<DirectWrite>().SetTextAlignment(currentTextAlignment_, formatData.first);
	Game::System<DirectWrite>().ImmediateDraw(text, formatData.first, formatData.second, x, y, width,height);
}

void mtgb::Text::ChangeFontSize(int size)
{
	currentDefaultFontSize_ = size;
	auto fontFormatData = GetOrCreateTextFormat(size);
	Game::System<DirectWrite>().ChangeFormat(fontFormatData.first, fontFormatData.second);
}

void mtgb::Text::ChangeTextAlignment(TextAlignment alignment)
{
	currentTextAlignment_ = alignment;
}

#endif

int mtgb::TextCache::GetOrCreateTextLayoutHandle(const std::wstring& _text, int _size)
{
	D2D1_SIZE_F layoutBoxSize = Game::System<Direct2D>().GetRenderTargetSize();
	return GetOrCreateTextLayoutHandle(_text, _size, layoutBoxSize.width, layoutBoxSize.height);
}

int mtgb::TextCache::GetOrCreateTextLayoutHandle(const std::wstring& _text, int _size, float _width, float _height)
{
	// text + size の範囲を取得
	auto& text_index = textLayoutData_->get<text_layout_order>();
	auto range		 = text_index.equal_range(std::make_tuple(_text, _size));
	for (auto it = range.first; it != range.second; ++it)
	{
		// width / height も一致するものを探す
		TextLayoutData* data = *it;
		if (data->width == _width && data->height == _height)
		{
			return data->handle; // 完全一致があれば再利用
		}
	}

	// 新規作成する
	FontFormatData* formatData = GetOrCreateTextFormat(_size);

	IDWriteTextLayout* pTextLayout = nullptr;
	Game::System<DirectWrite>().CreateTextLayout(_text, _width, _height, _size, formatData->format, &pTextLayout);

	int handle				   = ++nextHandle_;
	TextLayoutData* layoutData = new TextLayoutData { _text, _size, _width, _height, pTextLayout, handle };
	textLayoutData_->insert(layoutData);
	return handle;
}

TextLayoutData* mtgb::TextCache::GetTextLayoutData(int _handle)
{
	auto& handle_index = textLayoutData_->get<handle_order>();
	auto it			   = handle_index.find(_handle);

	if (it == handle_index.end())
		return nullptr;

	return *it;
}

mtgb::FontFormatData* mtgb::TextCache::GetOrCreateTextFormat(int _size)
{
	auto& size_index = fontFormatData_->get<font_size_order>();
	auto it			 = size_index.find(_size);

	if (it != size_index.end())
	{
		// 既存のフォーマットを返す
		return *it;
	}

	FontFormatData* fontFormatData = nullptr;
	Game::System<DirectWrite>().CreateFontFormatData(DEFAULT_FONT_FAMILY_NAME, _size, &fontFormatData);

	Game::System<DirectWrite>().ChangeFormat(fontFormatData->format, fontFormatData->pixelFontMetrics);
	fontFormatData_->insert(fontFormatData);

	return fontFormatData;
}
