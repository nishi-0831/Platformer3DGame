#pragma once
#include "Command.h"
#include "IComponentMemento.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include <typeindex>
#include <functional>
#include <vector>
#include "GameObject.h"
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
		void Execute() override;
		void Undo() override;
		void Redo() override;

		std::string Name() const override;
	private:
		void ApplyComponents();

		EntityId entityId_;
		std::string gameObjectName_;
		std::vector<IComponentMemento*> mementos_;
		const ComponentFactory& componentFactory_;
		CreateFunc createFunc_;
	};
}