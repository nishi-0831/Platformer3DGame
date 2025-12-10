#pragma once

#include <nlohmann/json.hpp>
#include <unordered_map>
#include <type_traits>
#include "JsonADLSerializers.h"

template <typename T>
constexpr bool is_builtin_type_v =
std::is_arithmetic_v<T> ||
std::is_same_v<T, std::string> ||
std::is_same_v<T, std::string_view> ||
std::is_enum_v<T>;

namespace JsonConverter::detail
{
	// to_json が存在するかチェック
	template <typename T, typename = void>
	struct has_to_json : std::false_type {};

	// nlohmann::json json = T value が可能か
	template <typename T>
	struct has_to_json<T, std::void_t<decltype(
		std::declval<nlohmann::json&>() = std::declval<const T&>()
		)>> : std::true_type {};

	// from_json が存在するかチェック
	template <typename T, typename = void>
	struct has_from_json : std::false_type {};

	// nlohmann::json::get<T>() が可能か
	template <typename T>
	struct has_from_json<T, std::void_t<decltype(
		std::declval<const nlohmann::json&>().get<T>()
		)>> : std::true_type {};

	template <typename T>
	constexpr bool has_adl_serializer_v =
		has_to_json<T>::value || has_from_json<T>::value;
}

namespace JsonConverter
{
	template<typename T>
	nlohmann::json Serialize(const T& _value);
	template<typename T>
	void Deserialize(T& _value, const nlohmann::json& _json);
	template<typename... Attrs>
	std::string GetDisplayName(const std::tuple<Attrs...>& _attrs) noexcept;
	template<typename T>
	std::string GetDisplayName() noexcept;
};

template <typename... Attrs>
std::string JsonConverter::GetDisplayName(const std::tuple<Attrs...>& _attrs) noexcept
{

	// パラメータの中からDisplayNameの型を探す
	if constexpr (std::disjunction_v<std::is_same<Attrs, typename ::DisplayName>...>)
	{
		return std::string(std::get<typename ::DisplayName>(_attrs)());
	}
	else
	{
		return "";
	}
}

template<typename T>
std::string JsonConverter::GetDisplayName() noexcept
{
	if constexpr (refl::is_reflectable<T>())
	{
		constexpr auto type = refl::reflect<T>();
		std::string displayName = GetDisplayName(type.attributes);
		if (displayName.empty() == false)
		{
			return displayName;
		}
		return std::string(type.name);
	}
	else
	{
		return "";
	}
}

template<typename T>
nlohmann::json JsonConverter::Serialize(const T& _value)
{
	using json = nlohmann::json;
	using Type = std::remove_pointer_t<std::remove_cvref_t<T>>;
	json data;
	if constexpr (is_builtin_type_v<Type>)
	{
		// 組み込み型
		data = _value;
	}
	else if constexpr (detail::has_to_json<Type>::value)
	{
		data = _value;
	}
	else if constexpr (refl::is_reflectable<Type>())
	{
		constexpr auto type = refl::reflect<Type>();
		constexpr auto members = type.members;
		refl::util::for_each(members, [&](auto _member)
			{
				if constexpr (refl::is_reflectable<decltype(_member(_value))>())
				{
					auto memberValue = _member(_value);
					std::string key = _member.name.c_str();
					if constexpr (is_builtin_type_v<decltype(memberValue)>)
					{
						data[key] = memberValue;
					}
					else
					{
						json memberJson = Serialize(memberValue);
						data[key] = memberJson;
					}
				}
				else if constexpr (refl::trait::is_field_v<decltype(_member)>)
				{
					std::string key = _member.name.c_str();
					data[key] = _member(_value);
				}
				else if constexpr (refl::trait::is_property_v<decltype(_member)>)
				{
					std::string key = refl::descriptor::get_display_name(_member);
					data[key] = _member.invoke(_value);
				}
			});
	}

	return data;
}

template<typename T>
void JsonConverter::Deserialize(T& _value, const nlohmann::json& _json)
{
	using json = nlohmann::json;
	using Type = std::remove_pointer_t<std::remove_cvref_t<T>>;

	if constexpr (is_builtin_type_v<Type>)
	{
		// 組み込み型
		_json.get_to(_value);
	}
	else if constexpr (detail::has_from_json<Type>::value)
	{
		_json.get_to(_value);
	}
	else if constexpr (refl::is_reflectable<Type>())
	{
		constexpr auto type = refl::reflect<Type>();
		constexpr auto members = type.members;
		refl::util::for_each(members, [&](auto _member)
			{
				std::string key = _member.name.c_str();
				if (_json.contains(key) == false)
					return;

				if constexpr (refl::is_reflectable<decltype(_member(_value))>())
				{
					auto& memberValue = _member(_value);
					if constexpr (is_builtin_type_v<decltype(memberValue)>)
					{
						_json.at(key).get_to(memberValue);
					}
					else
					{

						Deserialize(memberValue, _json.at(key));
					}
				}
				else if constexpr (refl::trait::is_field_v<decltype(_member)>)
				{
					_json.at(key).get_to(_member(_value));
				}
			});
	}
}

