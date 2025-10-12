#pragma once
#include <refl.hpp>
#include "../ImGui/imgui.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "ImGuiRenderer.h"
#include "Transform.h"
#include "ShowFunc.h"
#include "QuatToEuler.h"

struct Vector3Show : refl::attr::usage::type 
{
	void operator()(mtgb::Vector3* vec, const char* name) const
	{
		ImGui::InputFloat3(name, &vec->x);
	}
};


struct QuaternionSHow : refl::attr::usage::type
{
	void operator()(DirectX::XMVECTORF32* vec, const char* name) const
	{
		mtgb::Vector3 vec3 = mtgb::QuatToEuler(*vec);
		ImGui::InputFloat3(name, &vec3.x);
	}
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
