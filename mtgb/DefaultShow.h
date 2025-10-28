#pragma once
#include <type_traits>
#include <string>
#include <vector>
#include "../ImGui/imgui.h"
#include "MTStringUtility.h"

class Command;
namespace mtgb {
    template<typename T>
    Command* DefaultShow(T* value, const char* name);
}

// �e���v���[�g�������C�����C���Ŋ܂߂�
#include "DefaultShow.inl"