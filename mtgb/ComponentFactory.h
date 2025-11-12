#pragma once
#include <typeindex>
#include <unordered_map>
#include "ComponentConcept.h"

#include "ISystem.h"
#include "IComponentMemento.h"
#include "ComponentRegistry.h"
#include <functional>
namespace mtgb
{
	class ComponentFactory : public ISystem
	{
	public:
		void Initialize() override;
		void Update() override;

		// コンポーネント作成関数の型
		using CreateFunction = std::function<IComponentMemento*(EntityId _id)>;
		using CreateFromMementoFunction = std::function<void(const IComponentMemento& _memento)>;

		template<typename T, typename M>
		requires ComponentWithMementoT<T,M>
		void RegisterComponent();

		IComponentMemento* AddComponent(const std::type_index& _info, EntityId _id) const;
		bool AddComponentFromMemento(const IComponentMemento& _memento) const;

		void GetRegisteredTypes(std::vector<std::type_index>& _types) const
		{
			_types = types_;
		}
	private:
		std::unordered_map<std::type_index, CreateFunction> creators_;
		std::unordered_map<std::type_index, CreateFromMementoFunction> creatorsFromMemento_;
		std::vector<std::type_index> types_;

		
	};
	template<typename T, typename M>
		requires ComponentWithMementoT<T, M>
	inline void ComponentFactory::RegisterComponent()
	{
		std::type_index typeIdx(typeid(T));
		creators_[typeIdx] = [](EntityId _id)
			{
				 T& component = T::template Get(_id);
				 return component.SaveToMemento();
			};
		creatorsFromMemento_[typeIdx] = [](const IComponentMemento& _memento)
			{
				// memento をM型へダウンキャスト
				const M* pMemento = dynamic_cast<const M*>(&_memento);
				if (pMemento == nullptr)
				{
					// 必要ならエラー処理
					return;
				}

				// Entityに割り当てられているインデックスの取得を試みる
				auto componentIndex = Game::System<ComponentRegistry>().GetComponentIndex(_memento.GetEntityId(), _memento.GetComponentType());

				// エンティティのIdからコンポーネントを取得
				T* pComponent = nullptr;
				
				// インデックスが有効かチェック
				if (componentIndex.has_value())
				{
					// インデックスのコンポーネントの再利用を試みる
					// コンポーネントが自他含むEntityに利用されていないなら取得できる
					pComponent = T::template Reuse(componentIndex.value(), pMemento->GetEntityId());
				}
				// コンポーネントの再利用に失敗
				if (pComponent == nullptr)
				{
					// コンポーネントの取得、もしくは作成を試みる
					pComponent = &(T::template Get(pMemento->GetEntityId()));
				}

				// メメントからデータを復元
				if (pComponent != nullptr)
				{
					pComponent->RestoreFromMemento(*pMemento);
				}
			};
		types_.push_back(typeid(T));
	}
}