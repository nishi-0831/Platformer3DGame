#pragma once
#include <type_traits>
#include <string>
#include <vector>
#include "../ImGui/imgui.h"
#include "MTStringUtility.h"

class Command;
namespace mtgb
{
	template <typename T> Command* DefaultShow(T* _value, const char* _name);
}

// テンプレート実装をインラインで含める
#include "DefaultShow.inl"