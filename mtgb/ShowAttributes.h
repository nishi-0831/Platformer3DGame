#pragma once
#include <refl.hpp>
#include "../ImGui/imgui.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "ImGuiRenderer.h"
#include "Transform.h"
#include "ShowFunc.h"
#include "QuatToEuler.h"

class Command;
struct Vector3Show : refl::attr::usage::type 
{
	Command* operator()(mtgb::Vector3* vec, const char* name) const;
};


struct QuaternionSHow : refl::attr::usage::type
{
	Command* operator()(DirectX::XMVECTORF32* vec, const char* name) const;
};

struct Vector4Show : refl::attr::usage::type 
{
	Command* operator()(DirectX::XMVECTOR* _vec, const char* _name)const;
};

template <typename T>
struct Range : refl::attr::usage::member
{
	T Min;
	T Max;

	Range(T min, T max) : Min(min), Max(max) {}

	template<typename FieldType>
	void operator()(FieldType* instance, const char* name) const {
		ShowRange(instance, name, Min, Max);
	}
};

template <typename T>
struct ProxyFor : refl::attr::usage::type
{ using TargetType = T; };



template <typename T>
void ShowRange(T* instance, const char* name, T min, T max)
{
	if constexpr (std::is_same_v<T, int>)
	{
		ImGui::SliderInt(name, instance, min, max);
	}
	else if constexpr (std::is_same_v<T, float>)
	{
		ImGui::SliderFloat(name, instance, min, max);
	}
	else
	{
		ImGui::Text("Unknown : %s", name);
	}
}

template <typename T>
struct ReadOnly : refl::attr::usage::member
{
	template<typename FieldType>
	void operator()(FieldType* instance, const char* name) const {

	}
};
