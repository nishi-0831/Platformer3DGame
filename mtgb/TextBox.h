#pragma once
#include <mtgb.h>

namespace mtgb
{
	class TextBox : public GameObject
	{
	public:
		TextBox();
		TextBox(const TextRendererData& _data);
		TextBox(TextRendererData&& _data);
		TextRenderer* pTextRenderer_;
	private:
	};
}
