#pragma once
#include "Command.h"
#include "IComponentMemento.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include "GameObjectFactory.h"
#include <typeindex>
#include <functional>
#include <vector>
#include "GameObject.h"
#include <nlohmann/json.hpp>
#include <string>
namespace mtgb
{
	class DeleteGameObjectCommand : public Command
	{
	  public:
		DeleteGameObjectCommand(GameObject* _pGameObj, const GameObjectFactory& _gameObjectFactory);
		// Command を介して継承されました
		void Execute() override;
		void Undo() override;
		std::string Name() const override;
		mtgb::EntityId GetCommandTargetEntityId() const override;

	  private:
		const GameObjectFactory& gameObjectFactory_;
		EntityId targetEntityId_;
		std::string name_;
		GameObjectLayerFlag layerFlag_;
		GameObjectTag tag_;

		std::string typeName_;
		std::vector<IComponentMemento*> mementos_;
	};
} // namespace mtgb