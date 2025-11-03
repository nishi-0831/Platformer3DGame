#pragma once
#include "Command.h"
#include "IComponentMemento.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include <typeindex>
#include <functional>
#include <vector>
#include "GameObject.h"
#include <nlohmann/json.hpp>
namespace mtgb
{
	
	class GameObjectCreateCommand : public Command
	{
	public:
		using CreateFunc = std::function<GameObject* ()>;
		GameObjectCreateCommand(
			CreateFunc _createFunc,
			const ComponentFactory& _componentFactory
		);

		GameObjectCreateCommand(
			CreateFunc _createFunc,
			const ComponentFactory& _componentFactory,
			const nlohmann::json& _json
		);
		void Execute() override;
		void Undo() override;
		void Redo() override;

		std::string Name() const override;
	private:
		void ApplyComponents();
		void Deserialize(GameObject* _obj);
		EntityId entityId_;
		std::string gameObjectName_;
		std::vector<IComponentMemento*> mementos_;
		const ComponentFactory& componentFactory_;
		CreateFunc createFunc_;
		nlohmann::json json_;
		bool deserialized_;
	};
}