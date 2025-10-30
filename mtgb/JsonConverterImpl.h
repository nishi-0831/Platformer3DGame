#pragma once
#include "JsonConverter.h"
#include <refl.hpp>
#include <string>
#include "ReflectionInfo.h"
template <typename T>
constexpr bool false_v = false;




template<typename T>
static nlohmann::json JsonConverter::Serialize(T& _value)
{
	using json = nlohmann::json;
	using Type = std::remove_pointer_t<std::remove_cvref_t<T>>;
	json j;
	if constexpr (refl::is_reflectable<Type>())
	{
		constexpr auto type = refl::reflect<Type>();

		refl::util::for_each(type.members, [&](auto _member)
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
	}
	else
	{
		
		static_assert(false_v<T>);

	}
	return j;
}


