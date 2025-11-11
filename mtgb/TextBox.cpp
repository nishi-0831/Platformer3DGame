#include "stdafx.h"
#include "TextBox.h"

mtgb::TextBox::TextBox()
	: pTextRenderer_{nullptr}
{
}

mtgb::TextBox::TextBox(const TextRendererData& _data)
	: pTextRenderer_{Component<TextRenderer>()}
{
	pTextRenderer_->CopyData(_data);
}

mtgb::TextBox::TextBox(TextRendererData&& _data)
	: pTextRenderer_{ Component<TextRenderer>() }
{
	pTextRenderer_->MoveData(std::move(_data));
}
