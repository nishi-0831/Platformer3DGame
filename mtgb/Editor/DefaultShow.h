#pragma once
#include <type_traits>
#include <string>
#include <vector>
#include "ImGui/imgui.h"
#include "Utility/MTStringUtility.h"

class Command;
namespace mtgb
{
	/// <summary>
	/// 型に対応する表示関数を呼び出す
	/// </summary>
	/// <typeparam name="T">表示したい型</typeparam>
	/// <param name="_value">表示したい値</param>
	/// <param name="_name">ImGuiでの表示名</param>
	/// <returns>値を操作した場合、編集コマンドを返す。していないならnullptr</returns>
	template <typename T> Command* DefaultShow(T* _value, const char* _name);
} // namespace mtgb

// テンプレート実装をインラインで含める
#include "DefaultShow.inl"