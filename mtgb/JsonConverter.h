#pragma once
#include "stdafx.h"
#include <nlohmann/json.hpp>
#include <unordered_map>

template <typename T>
constexpr bool false_v = false;

namespace JsonConverter
{
	template<typename T>
	nlohmann::json Serialize(T& _value);

	void MergePatch(nlohmann::json& _target, nlohmann::json& _patch);



};






template<typename T>
nlohmann::json JsonConverter::Serialize(T& _value)
{
	using json = nlohmann::json;
	using Type = std::remove_pointer_t<std::remove_cvref_t<T>>;
	json j;
	if constexpr (refl::is_reflectable<Type>())
	{
		constexpr auto type = refl::reflect<Type>();
		constexpr auto members = type.members;
		refl::util::for_each(members, [&](auto _member)
			{
				if constexpr (refl::is_reflectable<decltype(_member(_value))>())
				{
					auto memberValue = _member(_value);
					json memberJson = Serialize(memberValue);
					j.merge_patch(memberJson);
				}
				else if constexpr (refl::trait::is_field_v<decltype(_member)>)
				{
					std::string key = std::string(_member.name.c_str());
					j[key] = _member(_value);
				}
				else if constexpr (refl::trait::is_property_v<decltype(_member)>)
				{
					std::string key = refl::descriptor::get_display_name(_member);
					j[key] = _member.invoke(_value);
				}
			});
		if (members.size == 0)
		{
			
		}
	}
	else
	{

		//static_assert(false_v<T>);

	}
	return j;
}

