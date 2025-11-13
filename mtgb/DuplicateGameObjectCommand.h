#pragma once
#include "Command.h"
#include "IComponentMemento.h"
#include "ComponentFactory.h"
#include "GameObject.h"
#include <vector>

namespace mtgb
{
	class DuplicateGameObjectCommand : public Command
	{
	public:
		using CreateFunc = std::function<GameObject* ()>;
		DuplicateGameObjectCommand(
			CreateFunc _createFunc,
			const ComponentFactory& _componentFactory,
			EntityId _srcEntityId
		);
		~DuplicateGameObjectCommand();
		// Command ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		void Execute() override;

		void Undo() override;
		void Redo() override;
		std::string Name() const override;

		mtgb::EntityId GetCommandTargetEntityId() const override;

	private:
		bool notSaveMementos_;
		void SaveToMementos();
		EntityId srcEntityId_;
		EntityId destEntityId_;
		std::string srcGameObjName_;
		std::string destGameObjName_;
		CreateFunc createFunc_;
		const ComponentFactory& componentFactory_;
		std::vector<IComponentMemento*> mementos_;
		std::vector<std::type_index> componentPoolTypes_;
	};
}