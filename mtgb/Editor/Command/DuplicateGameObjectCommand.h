#pragma once
#include "Command.h"
#include "Core/Component/IComponentMemento.h"
#include "Core/Component/ComponentFactory.h"
#include "Core/GameObject/GameObject.h"
#include "Core/GameObject/GameObjectFactory.h"

#include "Editor/GameObjectSnapshot.h"
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