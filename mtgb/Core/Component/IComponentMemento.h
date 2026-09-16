#pragma once
#include "Core/Entity.h"
#include <typeindex>
#include "ComponentConcept.h"
namespace mtgb
{
	class IComponentMemento
	{
	  public:
		virtual ~IComponentMemento()					 = default;
		virtual std::type_index GetComponentType() const = 0;
		virtual EntityId GetEntityId() const			 = 0;
	};

	/// <summary>
	/// Undo/Redoに使用する、コンポーネントのメメント
	/// </summary>
	/// <typeparam name="TComponent">コンポーネントの型</typeparam>
	/// <typeparam name="TState"Undo/Redoするうコンポーネントのデータ部分の型></typeparam>
	template <typename TComponent, typename TState> class ComponentMemento : public IComponentMemento
	{
	  public:
		using Component = TComponent;
		using State		= TState;

		ComponentMemento(EntityId _entityId, const State& _state)
			: state_ { _state }
			, entityId_ { _entityId }
		{
		}

		/// <summary>
		/// メメントのコンポーネントの型情報を返す
		/// </summary>
		/// <returns></returns>
		std::type_index GetComponentType() const override
		{
			return typeid(TComponent);
		}
		/// <summary>
		/// メメントのEntityIdを返す
		/// </summary>
		/// <returns></returns>
		EntityId GetEntityId() const override
		{
			return entityId_;
		}
		/// <summary>
		/// コンポーネントの以前のデータを返す
		/// </summary>
		/// <returns></returns>
		const State& GetState() const
		{
			return state_;
		}

	  private:
		State state_;
		EntityId entityId_;
	};
} // namespace mtgb