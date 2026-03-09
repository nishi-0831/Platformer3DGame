#pragma once
#include "Command.h"
#include "IComponentMemento.h"
#include "Entity.h"
#include "ComponentFactory.h"
#include <typeindex>
#include <string>
namespace mtgb
{
	/// <summary>
	/// コンポーネント追加コマンド
	/// </summary>
	class AddComponentCommand : public Command
	{
	  public:
		/// <summary>
		/// コンポーネント追加コマンド
		/// </summary>
		/// <param name="_entityId">エンティティのID</param>
		/// <param name="_typeIdx">コンポーネントの型情報</param>
		/// <param name="_memento"> nullの場合は_entityIdから既存のコンポーネントを取得、なければ新規作成しMementoに保存する/param>
		AddComponentCommand(
			EntityId _entityId,
			std::type_index _typeIdx,
			IComponentMemento* _pMemento
		);
		~AddComponentCommand();
		void Execute() override;
		void Undo() override;

		std::string Name() const override;
		EntityId GetCommandTargetEntityId() const override;

	  private:
		EntityId entityId_;
		std::type_index componentType_;
		IComponentMemento* pMemento_;

		void ApplyMemento();
		void RemoveComponent() const;
	};
} // namespace mtgb