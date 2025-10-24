#pragma once
#include "cmtgb.h"
#include "Game.h"
#include "ComponentPool.h"
#include "IComponentMemento.h"
namespace mtgb
{
	/// <summary>
	/// ゲームオブジェクトにつけるコンポーネントのインタフェース
	/// </summary>
	/// <typeparam name="ComponentPoolT">コンポーネントプール</typeparam>
	/// <typeparam name="ComponentT">コンポーネント</typeparam>
	template<typename ComponentT, typename MementoT>
	class IComponentWithMemento_2
	{
	public:

		IComponentWithMemento_2() :
			entityId_{ -1 }
		{
		}
		IComponentWithMemento_2(const EntityId _entityId);
		virtual ~IComponentWithMemento_2();

		virtual void Initialize() {}

		const EntityId GetEntityId() const { return entityId_; }

		MementoT* SaveToMemento();
		void RestoreFromMemento(const MementoT& _memento);

		ComponentT& GetComponent() { return component_; };
	private:
		virtual IComponentMemento* CreateMemento() = 0;
		EntityId entityId_;
		ComponentT component_;
	};

	template<typename ComponentT, typename MementoT>
	inline MementoT* IComponentWithMemento_2<ComponentT, MementoT>::SaveToMemento()
	{
		MementoT* memento = dynamic_cast<MementoT*>(CreateMemento());
		if (memento == nullptr) return nullptr;
		return memento;
	}

	template<typename ComponentT, typename MementoT>
	inline void IComponentWithMemento_2<ComponentT, MementoT>::RestoreFromMemento(const MementoT& _memento)
	{
		_memento.restore(component_);
	}

}
