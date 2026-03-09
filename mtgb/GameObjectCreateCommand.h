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
#include <concepts>
namespace mtgb
{

	class GameObjectCreateCommand : public Command
	{
	  public:
		using CreateFunc = std::function<GameObject*()>;
		 template<typename Func>
		 requires std::is_invocable_r_v<GameObject*,Func>
		GameObjectCreateCommand(Func&& _createFunc);

		template <typename Func>
			 requires std::is_invocable_r_v<GameObject*, Func>
		GameObjectCreateCommand(Func&& _createFunc, const nlohmann::json& _json);
		void Execute() override;
		void Undo() override;
		void Redo() override;

		std::string Name() const override;
		EntityId GetCommandTargetEntityId() const override;

	  private:
		void CreateGameObject();
		void Deserialize(GameObject* _obj);
		EntityId entityId_;
		std::string gameObjectName_;
		CreateFunc createFunc_;
		nlohmann::json json_;
	};
	template <typename Func>
		requires std::is_invocable_r_v<GameObject*, Func>
	inline GameObjectCreateCommand::GameObjectCreateCommand(Func&& _createFunc) 
		: entityId_ {INVALID_ENTITY}
		, createFunc_ { std::forward<Func>(_createFunc) }
		, json_ {}
	{
		
	}
	template <typename Func>
		requires std::is_invocable_r_v<GameObject*, Func>
	inline GameObjectCreateCommand::GameObjectCreateCommand(Func&& _createFunc, const nlohmann::json& _json)
		: GameObjectCreateCommand(std::forward<Func>(_createFunc))
	{
		json_ = _json;
	}
} // namespace mtgb