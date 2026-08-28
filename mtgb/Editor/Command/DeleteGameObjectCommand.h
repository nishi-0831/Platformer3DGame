#pragma once
#include "Command.h"
#include "Core/Component/IComponentMemento.h"
#include "Core/Entity.h"
#include "Core/Component/ComponentFactory.h"
#include "Editor/GameObjectSnapshot.h"
#include "Core/GameObject/GameObjectFactory.h"
#include "Editor/GameObjectSelectionEvent.h"
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