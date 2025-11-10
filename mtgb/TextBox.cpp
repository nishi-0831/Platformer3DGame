#include "stdafx.h"
#include "TextBox.h"

mtgb::TextBox::TextBox()
{
}

mtgb::TextBox::TextBox(const TextRendererData& _data)
	: pTextRenderer_{Component<TextRenderer>()}
{
	//TextRenderer::operator=(_data)
}

mtgb::TextBox::TextBox(TextRendererData&& _data)
{
}
