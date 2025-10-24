#pragma once
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include "ComponentConcept.h"

#include "Entity.h"
#include "IComponentMemento.h"
#include <functional>
namespace mtgb
{
	class ComponentFactory
	{
	public:
		// �R���|�[�l���g�쐬�֐��̌^
		using CreateFunction = std::function<IComponentMemento*(EntityId _id)>;
		using CreateFromMementoFunction = std::function<void(const IComponentMemento& _memento)>;

		template<typename T, typename M>
		requires MementoT<T,M>
		void RegisterComponent();

		IComponentMemento* AddComponent(const std::type_index& _info, EntityId _id);
		bool AddComponentFromMemento(const IComponentMemento& _memento);

		void GetRegisteredTypes(std::vector<std::type_index>& _types) const
		{
			_types = types_;
		}
	private:
		std::unordered_map<std::type_index, CreateFunction> creators_;
		std::unordered_map<std::type_index, CreateFromMementoFunction> creatorsFromMemento_;
		std::vector<std::type_index> types_;
	};
	template<typename T, typename M>
		requires MementoT<T, M>
	inline void ComponentFactory::RegisterComponent()
	{
		std::type_index typeIdx(typeid(T));
		creators_[typeIdx] = [](EntityId _id)
			{
				 T& component = T::template Get(_id);
				 return component.SaveToMemento();
			};
		creatorsFromMemento_[typeIdx] = [](const IComponentMemento& _memento)
			{
				// memento ��M�^�փ_�E���L���X�g
				const M* pMemento = dynamic_cast<const M*>(&_memento);
				if (pMemento == nullptr)
				{
					// �K�v�Ȃ�G���[����
					return;
				}

				// �G���e�B�e�B��Id����R���|�[�l���g���擾
				T& component = T::template Get(pMemento->GetEntityId());
				component.RestoreFromMemento(*pMemento);
			};
		types_.push_back(typeid(T));
	}
}