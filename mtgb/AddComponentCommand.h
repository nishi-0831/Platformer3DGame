#pragma once
#include "Command.h"
#include "IComponentMemento.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include <typeindex>

namespace mtgb
{
	/// <summary>
	/// �R���|�[�l���g�ǉ��R�}���h
	/// </summary>
	class AddComponentCommand : public Command
	{
	public:
		/// <summary>
		/// �R���|�[�l���g�ǉ��R�}���h
		/// </summary>
		/// <param name="_entityId">�G���e�B�e�B��ID</param>
		/// <param name="_typeIdx">�R���|�[�l���g�̌^���</param>
		/// <param name="_memento"> null�̏ꍇ��_entityId��������̃R���|�[�l���g���擾�A�Ȃ���ΐV�K�쐬��Memento�ɕۑ�����</param>
		/// <param name="_pComponentFactory"> �Q�ƂƂ��ĕێ�����̂ŁA�R�}���h��蒷����������K�v���� </param>
		AddComponentCommand(
			EntityId _entityId,
			const std::type_index& _typeIdx,
			IComponentMemento* _memento,
			const ComponentFactory& _pComponentFactory);
		~AddComponentCommand() = default;
		void Execute() override;
		void Undo() override;
		
		std::string Name() const override;
	private:
		EntityId entityId_;
		std::type_index componentType_;
		IComponentMemento* memento_;

		// ComponentFactory��null�ł��邱�Ƃ����e���Ȃ�
		const ComponentFactory& pComponentFactory_;

		
		void ApplyMemento();
		void RemoveComponent() const;

		// Command ����Čp������܂���
	};
}