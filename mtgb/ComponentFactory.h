#pragma once
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include "ComponentConcept.h"

#include "Entity.h"
#include <functional>
namespace mtgb
{
	class ComponentFactory
	{
	public:
		// コンポーネント作成関数の型
		using CreateFunction = std::function<void(EntityId _id)>;

		template<ComponentT T>
		void RegisterComponent();

		bool CreateComponent(const std::type_index& _info, EntityId _id);

		void GetRegisteredTypes(std::vector<std::type_index>& _types) const
		{
			_types = types_;
		}
	private:
		std::unordered_map<std::type_index, CreateFunction> creators_;
		std::vector<std::type_index> types_;
	};
	template<ComponentT T>
	inline void ComponentFactory::RegisterComponent()
	{
		std::type_index typeIdx(typeid(T));
		creators_[typeIdx] = [](EntityId _id)
			{
				 T::template Get(_id);
			};

		types_.push_back(typeid(T));
	}
}