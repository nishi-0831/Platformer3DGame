#pragma once
#include "ReflectionMacro.h"

namespace mtgb
{
	/// <summary>
	/// テキストを矩形領域内のどこに表示するか
	/// </summary>
	enum class [[MT_ENUM()]] TextAlignment
	{
		TOP_LEFT,	 // 左上に配置
		CENTER,		 // 中央に配置
		BOTTOM_LEFT, // 左下の配置
		MIDDLE_LEFT, // 左中央の配置
	};
} // namespace mtgb
#include "TextAlignment.generatedEnum.h"