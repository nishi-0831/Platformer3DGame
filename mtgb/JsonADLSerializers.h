#pragma once
#include "nlohmann/json.hpp"
#include <bitset>
#include "GameObjectLayer.h"
namespace nlohmann
{
	template <std::size_t N>
	struct adl_serializer<std::bitset<N>>
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

	template<typename EnumStructT>
	struct adl_serializer< ::mtbit::BitFlag<EnumStructT> >
	{
		using BitsetT = std::bitset< (sizeof(EnumStructT) * 8) >;

		static void to_json(nlohmann::json& j, const ::mtbit::BitFlag<EnumStructT>& flag)
		{
			// std::bitset‚Ìadl_serializer‚ðŽg—p
			j = flag.Value();
		}

		static void from_json(const nlohmann::json& j, ::mtbit::BitFlag<EnumStructT>& flag)
		{
			BitsetT bs;
			
			j.get_to(bs);
			flag = ::mtbit::BitFlag<EnumStructT>(bs);
		}
	};
}