#pragma once
#include "IComponent.h"
#include "ComponentConcept.h"
#include "IComponentMemento.h"

#include "nlohmann/json.hpp"
#include "JsonConverter.h"
#define UseConcept 0
namespace mtgb
{
	template<typename TDerived,typename TComponentPool,typename TState>
	
	class StatefulComponent : public TState , public IComponent<TComponentPool, TDerived>
	{
	public:	
		using IComponent<TComponentPool, TDerived>::IComponent;
		using TState::TState;
		using Memento = ComponentMemento<TDerived, TState>;
		using ComponentPool = TComponentPool;
		virtual ~StatefulComponent() {};
		virtual void Initialize() override {};
		StatefulComponent();
		StatefulComponent(TState&& _data, EntityId _entityId);
		/// <summary>
		/// 状態、EntityIdをメメントに保存、返す
		/// </summary>
		/// <returns>返されるメメント</returns>
		Memento* SaveToMemento()
		{
			OnPreSave();
			return new Memento(this->GetEntityId(), static_cast<const TState&>(*this));
		}
		void CopyData(const TState& _data)
		{
			static_cast<TState&>(*this) =_data;
		}
		void MoveData(TState&& _data)
		{
			static_cast<TState&>(*this) = std::move(_data);
		}
		/// <summary>
		/// メメントから復元を行う
		/// </summary>
		/// <param name="_memento">復元に使われるメメント</param>
		void RestoreFromMemento(const Memento& _memento)
		{
			const TState& data = _memento.GetState();

			CopyData(data);
			OnPostRestore();
		}

		/// <summary>
		/// シリアライズを行い、JSONを返す
		/// </summary>
		/// <returns></returns>
		nlohmann::json Serialize()
		{

			OnPreSave();
			// 状態のみを持たせたTDataに変換
			TState& data = static_cast<TState&>(*this);

			// シリアライズ
			nlohmann::json dataJson = JsonConverter::Serialize<TState>(data);

			// キーとなるTDataの名前を取得
			std::string key = JsonConverter::GetDisplayName<TState>();

			if (key.empty() == false)
			{
				// キー : 状態 としてJSONに書き込む
				nlohmann::json ret;
				ret[key] = dataJson;
				return ret;
			}

			// キーとなるTDataの名前が取得できなかった場合、状態をそのまま返す
			return dataJson;
		}

		static Memento* Deserialize(EntityId _entityId, const nlohmann::json& _json)
		{
			std::string key = JsonConverter::GetDisplayName<TState>();
			if (key.empty())
			{
				if constexpr (refl::is_reflectable<TState>())
				{
					auto typeDescriptor = refl::reflect<TState>();
					key = typeDescriptor.name.c_str();
				}
			}
			nlohmann::json stateJson = _json.at(key);
			
			TState state;
			JsonConverter::Deserialize(state, stateJson);
			return new Memento(_entityId, state);
		}

		static std::string DisplayName()
		{
			std::string componentName = JsonConverter::GetDisplayName<TState>();
			if (componentName.empty())
			{
				if constexpr (refl::is_reflectable<TState>())
				{
					auto typeDescriptor = refl::reflect<TState>();
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
		/// <summary>
		/// メメント保存、シリアライズの直前に呼ばれるフック
		/// </summary>
		virtual void OnPreSave()
		{
			// デフォルトでは何もしない
		}

	private:
	};
	template<typename TDerived, typename TComponentPool, typename TState>
	inline StatefulComponent<TDerived, TComponentPool, TState>::StatefulComponent()
		: TState()
		, IComponent<TComponentPool, TDerived>()
	{
	}
	template<typename TDerived, typename TComponentPool, typename TState>
	inline StatefulComponent<TDerived, TComponentPool, TState>::StatefulComponent(TState&& _state, EntityId _entityId)
		: TState{std::move(_state)}
		, IComponent<TComponentPool, TDerived>{_entityId}

	{
	}
	
}