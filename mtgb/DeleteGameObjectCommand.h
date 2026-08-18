#pragma once
#include "Command.h"
#include "IComponentMemento.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include "GameObjectSnapshot.h"
#include "GameObjectFactory.h"
#include "GameObjectSelectionEvent.h"
#include <typeindex>
#include <functional>
#include <vector>
#include <nlohmann/json.hpp>
#include <string>
namespace mtgb
{
	class DeleteGameObjectCommand : public Command
	{
	  public:
		DeleteGameObjectCommand(std::span<EntityId> _entityIds, const GameObjectFactory& _gameObjectFactory);
		// Command を介して継承されました
		void Execute() override;
		void Undo() override;
		std::string Name() const override;

	  private:
		const GameObjectFactory& gameObjectFactory_;
		std::vector<GameObjectSnapshot> snapshots_;
	};
} // namespace mtgb