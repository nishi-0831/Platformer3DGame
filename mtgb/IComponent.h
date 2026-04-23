#pragma once
#include "cmtgb.h"
#include "Game.h"
#include "ComponentPool.h"
#include <nlohmann/json.hpp>

namespace mtgb
{
	/// <summary>
	/// ゲームオブジェクトにつけるコンポーネントのインタフェース
	/// </summary>
	/// <typeparam name="ComponentPoolT">コンポーネントプール</typeparam>
	/// <typeparam name="ComponentT">コンポーネント</typeparam>
	template <class ComponentPoolT, typename ComponentT> class IComponent
	{
	  public:
		using Pool = ComponentPoolT;
		friend ComponentPoolT;
		friend ComponentPool<ComponentT, ComponentPoolT>;
		IComponent()
			: entityId_ { INVALID_ENTITY }
		{
		}
		IComponent(EntityId _entityId);
		virtual ~IComponent();
		IComponent& operator=(const IComponent&) = default;
		static ComponentT& Get(EntityId _entityId);
		template <typename... Args> static ComponentT& Get(EntityId _entityId, Args&&... _args);

		static ComponentT* Reuse(size_t _index, EntityId _entityId);
		static ComponentT* Reuse(EntityId _entityId);
		virtual void Initialize() {}
		/// <summary>
		/// <para> ゲームオブジェクトから取り外されたときに呼ばれる </para>
		/// <para> コンポーネントの状態を初期化するのに使う </para>
		/// </summary>
		virtual void Reset() {}
		EntityId GetEntityId() const
		{
			return entityId_;
		}

	  private:
		EntityId entityId_;
	};

	template <class ComponentPoolT, typename ComponentT>
	inline IComponent<ComponentPoolT, ComponentT>::IComponent(EntityId _entityId)
		: entityId_ { _entityId }
	{
	}

	template <class ComponentPoolT, typename ComponentT> inline IComponent<ComponentPoolT, ComponentT>::~IComponent() {}

	template <class ComponentPoolT, typename ComponentT>
	template <typename... Args>
	inline ComponentT& IComponent<ComponentPoolT, ComponentT>::Get(EntityId _entityId, Args&&... _args)
	{
		return Game::System<ComponentPoolT>().Get(_entityId, std::forward<Args>(_args)...);
	}

	template <class ComponentPoolT, typename ComponentT>
	inline ComponentT& IComponent<ComponentPoolT, ComponentT>::Get(EntityId _entityId)
	{
		return Game::System<ComponentPoolT>().Get(_entityId);
	}

	template <class ComponentPoolT, typename ComponentT>
	inline ComponentT* IComponent<ComponentPoolT, ComponentT>::Reuse(size_t _index, EntityId _entityId)
	{
		return Game::System<ComponentPoolT>().Reuse(_index, _entityId);
	}

	template <class ComponentPoolT, typename ComponentT>
	inline ComponentT* IComponent<ComponentPoolT, ComponentT>::Reuse(EntityId _entityId)
	{
		ComponentT* result = nullptr;

		auto componentIndex = Game::System<ComponentRegistry>().GetComponentIndex(_entityId, typeid(ComponentT));
		if (componentIndex.has_value())
		{
			result = Game::System<ComponentPoolT>().Reuse(componentIndex.value(), _entityId);
		}
		return result;
	}

} // namespace mtgb
