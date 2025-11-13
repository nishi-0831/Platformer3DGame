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
		/// <param name="_memento"> nullの場合は_entityIdから既存のコンポーネントを取得、なければ新規作成しMementoに保存する</param>
		/// <param name="_pComponentFactory"> 参照として保持するので、コマンドより長く生存する必要あり </param>
		AddComponentCommand(
			EntityId _entityId,
			const std::type_index& _typeIdx,
			IComponentMemento* _memento,
			const ComponentFactory& _pComponentFactory);
		~AddComponentCommand() = default;
		void Execute() override;
		void Undo() override;
		
		std::string Name() const override;
		EntityId GetCommandTargetEntityId() const override;
	private:
		EntityId entityId_;
		size_t componentPoolIndex_;
		std::type_index componentType_;
		IComponentMemento* memento_;

		// ComponentFactoryがnullであることを許容しない
		const ComponentFactory& componentFactory_;

		
		void ApplyMemento();
		void RemoveComponent() const;

		// Command を介して継承されました
	};
}