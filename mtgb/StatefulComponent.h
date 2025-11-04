#pragma once
#include "IComponent.h"
#include "ComponentConcept.h"
#include "IComponentMemento.h"

#include "nlohmann/json.hpp"
#include "JsonConverter.h"
#define UseConcept 0
namespace mtgb
{
	template<typename TDerived,typename TComponentPool,typename TData>
	
	class StatefulComponent : public IComponent<TComponentPool, TDerived>
	{
	public:	
		using IComponent<TComponentPool, TDerived>::IComponent;
		using Memento = ComponentMemento<TDerived, TData>;
		
		virtual ~StatefulComponent() {};
		virtual void Initialize() override {};

		Memento* SaveToMemento() const
		{
			// conceptで検証済みなのでstatic_cast
			return new Memento(this->GetEntityId(), static_cast<const TDerived&>(*this));
		}

		void RestoreFromMemento(const Memento& _memento)
		{
			// conceptで検証済みなのでstatic_cast
			auto& derivedRef = static_cast<TDerived&>(*this);
			const TData& data = _memento.GetData();

			static_cast<TData&>(derivedRef) = data;
			OnPostRestore();
		}

		nlohmann::json Serialize()
		{
			auto& derivedRef = static_cast<TDerived&>(*this);
			TData& data = static_cast<TData&>(derivedRef);

			nlohmann::json dataJson = JsonConverter::Serialize<TData>(data);

			std::string key = JsonConverter::GetDisplayName<TData>();
			if (key.empty() == false)
			{
				nlohmann::json ret;
				ret[key] = dataJson;
				return ret;
			}
			return dataJson;
		}

		static Memento* Deserialize(EntityId _entityId, const nlohmann::json& _json)
		{
			std::string key = JsonConverter::GetDisplayName<TData>();
			if (key.empty())
			{
				if constexpr (refl::is_reflectable<TData>())
				{
					auto typeDescriptor = refl::reflect<TData>();
					key = typeDescriptor.name.c_str();
				}
			}
			nlohmann::json dataJson = _json.at(key);
			
			TData data;
			JsonConverter::Deserialize(data, dataJson);
			return new Memento(_entityId, data);
		}

		static std::string DisplayName()
		{
			std::string componentName = JsonConverter::GetDisplayName<TData>();
			if (componentName.empty())
			{
				if constexpr (refl::is_reflectable<TData>())
				{
					auto typeDescriptor = refl::reflect<TData>();
					componentName = typeDescriptor.name.c_str();
				}
			}
			return componentName;
		}

	protected:
		/// <summary>
		/// <para> Mementoから復元が行われた際に呼ばれるフック </para>
		/// </summary>
		virtual void OnPostRestore()
		{
			// デフォルトでは何もしない
		}
	private:
	};
}