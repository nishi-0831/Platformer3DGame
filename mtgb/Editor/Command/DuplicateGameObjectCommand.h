#pragma once
#include "Command.h"
#include "IComponentMemento.h"
#include "ComponentFactory.h"
#include "GameObject.h"
#include "GameObjectFactory.h"

#include "GameObjectSnapshot.h"
#include <vector>

namespace mtgb
{
	class DuplicateGameObjectCommand : public Command
	{
	  public:
		DuplicateGameObjectCommand(std::span<EntityId> _entityIds, const GameObjectFactory& _gameObjectFactory);
		~DuplicateGameObjectCommand();
		// Command を介して継承されました
		void Execute() override;

		void Undo() override;
		void Redo() override;
		std::string Name() const override;

	  private:
		struct DuplicateSnapshot : GameObjectSnapshot
		{
			EntityId destEntityId_;
			std::vector<std::type_index> componentPoolTypes;
			bool notSaveMementos;
		};
		void SaveToMementos(DuplicateSnapshot& _snapshot);
		std::vector<DuplicateSnapshot> snapshots_;
		const GameObjectFactory& gameObjectFactory_;
	};
} // namespace mtgb