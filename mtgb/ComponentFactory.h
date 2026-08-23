#pragma once
#include "ISystem.h"
#include <typeindex>
#include <unordered_map>
#include "ComponentConcept.h"

#include "IComponentMemento.h"
#include "ComponentRegistry.h"
#include <functional>
#include <span>
namespace mtgb
{
	class Game;

	/// <summary>
	/// コンポーネントを作成するコールバックを管理するクラス
	/// </summary>
	class ComponentFactory
	{
	  public:
		// コンポーネント作成関数の型
		using CreateFunction			= std::function<IComponentMemento*(EntityId _id)>;
		using CreateFromMementoFunction = std::function<void(const IComponentMemento& _memento)>;
		/// <summary>
		/// コンポーネントの作成関数を登録する
		/// </summary>
		/// <typeparam name="T"></typeparam>
		template <typename T> void RegisterComponent();

		/// <summary>
		/// コンポーネントを追加する。そして、初期値をメメントとして返す
		/// </summary>
		/// <param name="_info">コンポーネントのtype_index</param>
		/// <param name="_id">エンティティのID</param>
		/// <returns>メメントのポインタ</returns>
		IComponentMemento* AddComponent(const std::type_index& _info, EntityId _id) const;
		/// <summary>
		/// メメントを使ってコンポーネントを追加する
		/// </summary>
		/// <param name="_memento">追加に使用するコンポーネント</param>
		/// <returns>追加に成功した場合true、失敗したらfalse</returns>
		bool AddComponentFromMemento(const IComponentMemento& _memento) const;
		/// <summary>
		/// 登録されているコンポーネントのtype_indexの配列を返す
		/// </summary>
		/// <returns></returns>
		std::span<const std::type_index> GetRegisteredTypes()
		{
			return types_;
		}

	  private:
		/// コンポーネントのtype_index : コンポーネント追加関数
		std::unordered_map<std::type_index, CreateFunction> creators_;
		/// コンポーネントのtype_index : メメントを使ってコンポーネント追加する関数
		std::unordered_map<std::type_index, CreateFromMementoFunction> creatorsFromMemento_;
		/// コンポーネントのtype_index配列
		std::vector<std::type_index> types_;
	};

	template <typename T> void ComponentFactory::RegisterComponent()
	{
		using Memento = T::Memento;

		std::type_index typeIdx(typeid(T));
		creators_[typeIdx] = [](EntityId _id)
		{
			T& component = T::Get(_id);
			return component.SaveToMemento();
		};
		creatorsFromMemento_[typeIdx] = [](const IComponentMemento& _memento)
		{
			// memento をM型へダウンキャスト
			const Memento* pMemento = dynamic_cast<const Memento*>(&_memento);
			if (pMemento == nullptr)
			{
				// 必要ならエラー処理
				return;
			}

			// Entityに割り当てられているインデックスの取得を試みる
			auto componentIndex = Game::template System<ComponentRegistry>().GetComponentIndex(
				_memento.GetEntityId(),
				_memento.GetComponentType()
			);

			// エンティティのIdからコンポーネントを取得
			T* pComponent = nullptr;

			// インデックスが有効かチェック
			if (componentIndex.has_value())
			{
				// インデックスのコンポーネントの再利用を試みる
				// コンポーネントが自他含むEntityに利用されていないなら取得できる
				pComponent = T::Reuse(componentIndex.value(), pMemento->GetEntityId());
			}
			// コンポーネントの再利用に失敗
			if (pComponent == nullptr)
			{
				// コンポーネントの取得、もしくは作成を試みる
				pComponent = &(T::Get(pMemento->GetEntityId()));
			}

			// メメントからデータを復元
			if (pComponent != nullptr)
			{
				pComponent->RestoreFromMemento(*pMemento);
			}
		};
		types_.push_back(typeid(T));
	}
}; // namespace mtgb