#pragma once
#include "nlohmann/json.hpp"
#include <bitset>
#include "GameObjectLayer.h"
#include "Color.h"
namespace nlohmann
{
	// adl_serializerを特殊化してシリアライズ、デシリアライズ関数を定義する

	template <std::size_t N> struct adl_serializer<std::bitset<N>>
	{
		static void to_json(json& _j, const std::bitset<N>& _bitset)
		{
			_j = _bitset.to_ulong();
		}

		static void from_json(const json& _j, std::bitset<N>& _bitset)
		{
			if (_j.is_number_unsigned() == false)
				return;

			_bitset.reset();

			unsigned long long v = _j.get<unsigned long long>();

			for (std::size_t i = 0; i < N && (i < 64); i++)
			{
				if (v & (1ull << i))
				{
					_bitset.set(i);
				}
			}
		}
	};

	template <typename EnumStructT> struct adl_serializer<::mtbit::BitFlag<EnumStructT>>
	{
		using BitsetT = std::bitset<(sizeof(EnumStructT) * 8)>;

		static void to_json(nlohmann::json& _j, const ::mtbit::BitFlag<EnumStructT>& _flag)
		{
			// std::bitsetのadl_serializerを使用
			_j = _flag.Value();
		}

		static void from_json(const nlohmann::json& _j, ::mtbit::BitFlag<EnumStructT>& _flag)
		{
			BitsetT bs;

			_j.get_to(bs);
			_flag = ::mtbit::BitFlag<EnumStructT>(bs);
		}
	};

	template <> struct adl_serializer<::mtgb::Color>
	{
		static void to_json(nlohmann::json& _j, const ::mtgb::Color& _color)
		{
			_j = {
				{"r", _color.component[static_cast<int32_t>(::mtgb::Color::Component::RED)]},
				{"g", _color.component[static_cast<int32_t>(::mtgb::Color::Component::GREEN)]},
				{"b", _color.component[static_cast<int32_t>(::mtgb::Color::Component::BLUE)]},
				{"a", _color.component[static_cast<int32_t>(::mtgb::Color::Component::ALPHA)]}
			};
		}

		static void from_json(const nlohmann::json& _j, ::mtgb::Color& _color)
		{
			uint8_t r = _j.value("r", uint8_t(0));
			uint8_t g = _j.value("g", uint8_t(0));
			uint8_t b = _j.value("b", uint8_t(0));
			uint8_t a = _j.value("a", uint8_t(255));

			_color = ::mtgb::Color(r, g, b, a);
		}
	};

} // namespace nlohmann