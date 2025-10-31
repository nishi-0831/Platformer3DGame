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
		requires ComponentWithMementoT<T,M>
		void RegisterComponent();

		IComponentMemento* AddComponent(const std::type_index& _info, EntityId _id) const;
		bool AddComponentFromMemento(const IComponentMemento& _memento) const;

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
		requires ComponentWithMementoT<T, M>
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

				// �ȑO�ɍ쐬���ꂽ�ۂɊ��蓖�Ă�ꂽ�C���f�b�N�X�̎擾�����݂�
				auto componentIndex = IComponentPool::GetComponentIndex(_memento.GetEntityId(), _memento.GetComponentType());

				// �G���e�B�e�B��Id����R���|�[�l���g���擾
				T* pComponent = nullptr;
				
				// �C���f�b�N�X���L�����`�F�b�N
				if (componentIndex.has_value())
				{
					// �C���f�b�N�X�̃R���|�[�l���g�̍ė��p�����݂�
					// �R���|�[�l���g���ǂ�Entity�ɂ����p����Ă��Ȃ��Ȃ�擾�ł���
					pComponent = T::template Reuse(componentIndex.value(), pMemento->GetEntityId());
				}
				// �R���|�[�l���g�̍ė��p�Ɏ��s�A�������͈ȑO�ɍ쐬����Ă��Ȃ�
				if (pComponent == nullptr)
				{
					// �R���|�[�l���g�̎擾�A�������͍쐬�����݂�
					pComponent = &(T::template Get(pMemento->GetEntityId()));
				}

				// �������g����f�[�^�𕜌�
				if (pComponent != nullptr)
				{
					pComponent->RestoreFromMemento(*pMemento);
				}
			};
		types_.push_back(typeid(T));
	}
}