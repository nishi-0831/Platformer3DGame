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
		using CreateFunc = std::function<GameObject*()>;
		template <typename Func>
			requires std::is_invocable_r_v<GameObject*, Func>
		DuplicateGameObjectCommand(Func&& _createFunc, EntityId _srcEntityId);
		~DuplicateGameObjectCommand();
		// Command を介して継承されました
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
		std::vector<IComponentMemento*> mementos_;
		std::vector<std::type_index> componentPoolTypes_;
	};
	template <typename Func>
		requires std::is_invocable_r_v<GameObject*, Func>
	inline DuplicateGameObjectCommand::DuplicateGameObjectCommand(Func&& _createFunc, EntityId _srcEntityId)
		: notSaveMementos_ { true }
		, srcEntityId_ { _srcEntityId }
		, createFunc_ { std::forward<Func>(_createFunc) }
	{
		GameObject* src = Game::System<SceneSystem>().GetActiveScene()->GetGameObject(srcEntityId_);
		if (src == nullptr)
			return;

		srcGameObjName_			  = src->GetName();
		std::string classTypeName = src->GetClassTypeName();

		std::optional<std::vector<std::type_index>> componentPoolTypes =
			Game::System<ComponentRegistry>().GetComponentPoolTypes(srcEntityId_);

		if (componentPoolTypes.has_value() == false)
			return;

		componentPoolTypes_ = std::move(componentPoolTypes.value());
	}
} // namespace mtgb