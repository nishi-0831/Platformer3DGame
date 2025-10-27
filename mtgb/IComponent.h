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
	template<class ComponentPoolT, typename ComponentT>
	class IComponent
	{
		friend ComponentPoolT;
		friend ComponentPool<ComponentT>;
	public:
		using ComPool = ComponentPoolT;
		IComponent() :
			entityId_{ -1 }
		{}
		IComponent(const EntityId _entityId);
		virtual ~IComponent();

		static ComponentT& Get(const EntityId _entityId);
		template<typename... Args>
		static ComponentT& Get(const EntityId _entityId, Args&&... _args);
		
		static bool Reuse(ComponentT* _pComponent, size_t _index, EntityId _entityId);
		virtual void Initialize() {}

		const EntityId GetEntityId() const { return entityId_; }

	private:
		EntityId entityId_;
	};

	template<class ComponentPoolT, typename ComponentT>
	inline IComponent<ComponentPoolT, ComponentT>::IComponent(const EntityId _entityId) :
		entityId_{ _entityId }
	{
		//Game::System<ComponentPoolT>().Register(_entityId);
	}

	template<class ComponentPoolT, typename ComponentT>
	inline IComponent<ComponentPoolT, ComponentT>::~IComponent()
	{
		//Game::System<ComponentPoolT>().UnRegister(entityId_);
	}

	template<class ComponentPoolT, typename ComponentT>
	template<typename... Args>
	inline ComponentT& IComponent<ComponentPoolT, ComponentT>::Get(const EntityId _entityId, Args&&... _args)
	{
		return Game::System<ComponentPoolT>().Get(_entityId, std::forward<Args>(_args)...);
	}

	template<class ComponentPoolT, typename ComponentT>
	inline ComponentT& IComponent<ComponentPoolT, ComponentT>::Get(const EntityId _entityId)
	{
		return Game::System<ComponentPoolT>().Get(_entityId);
	}

	template<class ComponentPoolT, typename ComponentT>
	inline bool IComponent<ComponentPoolT, ComponentT>::Reuse(ComponentT* _pComponent, size_t _index, EntityId _entityId)
	{
		return Game::System<ComponentPoolT>().Reuse(_pComponent, _index, _entityId);
	}

}
