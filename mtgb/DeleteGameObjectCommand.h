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
		DeleteGameObjectCommand(GameObject* pGameObj,const GameObjectFactory& _gameObjectFactory, const ComponentFactory& _componentFactory);
		// Command ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
		void Execute() override;
		void Undo() override;
		std::string Name() const override;
		mtgb::EntityId GetCommandTargetEntityId() const override;
	private:
		const ComponentFactory& componentFactory_;
		const GameObjectFactory& gameObjectFactory_;
		EntityId targetEntityId_;
		std::string name_;
		GameObjectLayerFlag layerFlag_;
		GameObjectTag tag_;
		bool isNotCalledStart_;

		std::string typeName_;
		std::vector<IComponentMemento*> mementos_;

	};
}