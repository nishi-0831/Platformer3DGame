#pragma once
#include <type_traits>
#include <string>
#include <vector>
#include "../ImGui/imgui.h"
#include "MTStringUtility.h"

namespace mtgb {
    template<typename T>
    void DefaultShow(T* value, const char* name);
}

// �e���v���[�g�������C�����C���Ŋ܂߂�
#include "DefaultShow.inl"