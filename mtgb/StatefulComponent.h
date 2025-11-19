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
	
	class StatefulComponent : public TData , public IComponent<TComponentPool, TDerived>
	{
	public:	
		using IComponent<TComponentPool, TDerived>::IComponent;
		using TData::TData;
		using Memento = ComponentMemento<TDerived, TData>;
		using ComponentPool = TComponentPool;
		virtual ~StatefulComponent() {};
		virtual void Initialize() override {};
		StatefulComponent();
		StatefulComponent(TData&& _data, EntityId _entityId);
		Memento* SaveToMemento() const
		{
			return new Memento(this->GetEntityId(), static_cast<const TData&>(*this));
		}
		void CopyData(const TData& _data)
		{
			static_cast<TData&>(*this) =_data;
		}
		void MoveData(TData&& _data)
		{
			static_cast<TData&>(*this) = std::move(_data);
		}
		void RestoreFromMemento(const Memento& _memento)
		{
			const TData& data = _memento.GetData();

			CopyData(data);
			OnPostRestore();
		}

		nlohmann::json Serialize()
		{
			TData& data = static_cast<TData&>(*this);

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
	template<typename TDerived, typename TComponentPool, typename TData>
	inline StatefulComponent<TDerived, TComponentPool, TData>::StatefulComponent()
		: TData()
		, IComponent<TComponentPool, TDerived>()
	{
	}
	template<typename TDerived, typename TComponentPool, typename TData>
	inline StatefulComponent<TDerived, TComponentPool, TData>::StatefulComponent(TData&& _data, EntityId _entityId)
		: TData{std::move(_data)}
		, IComponent<TComponentPool, TDerived>{_entityId}

	{
	}
	
}