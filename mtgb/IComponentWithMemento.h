#pragma once
#include "cmtgb.h"
#include "Game.h"
#include "ComponentPool.h"

namespace mtgb
{
	/// <summary>
	/// �Q�[���I�u�W�F�N�g�ɂ���R���|�[�l���g�̃C���^�t�F�[�X
	/// </summary>
	/// <typeparam name="ComponentPoolT">�R���|�[�l���g�v�[��</typeparam>
	/// <typeparam name="ComponentT">�R���|�[�l���g</typeparam>
	template<typename ComponentT,typename MementoT>
	class IComponentWithMemento
	{
	public:
		
		IComponentWithMemento() :
			entityId_{ -1 }
		{
		}
		IComponentWithMemento(const EntityId _entityId);
		virtual ~IComponentWithMemento();

		virtual void Initialize() {}

		const EntityId GetEntityId() const { return entityId_; }

		MementoT* SaveToMemento();
		void RestoreFromMemento(const MementoT& _memento);

		ComponentT& GetComponent() { return component_; };
	private:
		EntityId entityId_;
		ComponentT component_;
	};

	
	

	

	template<typename ComponentT, typename MementoT>
	inline MementoT* IComponentWithMemento<ComponentT, MementoT>::SaveToMemento()
	{
		return new MementoT(component_);
	}

	template<typename ComponentT, typename MementoT>
	inline void IComponentWithMemento<ComponentT, MementoT>::RestoreFromMemento(const MementoT& _memento)
	{
		_memento.restore(component_);
	}

}
