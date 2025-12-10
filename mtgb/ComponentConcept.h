#pragma once
#include <concepts>
#include <type_traits>

namespace mtgb
{
	// ============================================================
	// コンポーネント関連のコンセプト
	// ============================================================

	/// <summary>
	/// T は Component インターフェースを実装している
	/// （Get(EntityId) 静的メソッドを持つ）
	/// </summary>
	/*template <typename T>
	concept ComponentT = requires(T _com, EntityId _id)
	{
		T::Get(_id);
	};*/

	/// <summary>
	/// T は単純なメモリレイアウトのコンポーネント状態構造体
	/// </summary>
	template<typename T>
	concept ComponentDataT =
		std::is_standard_layout_v<T>;

	// ============================================================
	// 継承関係のコンセプト
	// ============================================================

	/// <summary>
	/// TDerived は TData を公開継承している
	/// → static_cast<TData&>(derived) が安全
	/// 
	/// CRTPなどで TDerivedが不完全型の状態で評価される場合があるため、
	/// ポインタをstatic_cast可能かで判定する
	/// </summary>
	template <typename TDerived, typename TData>
	concept DerivedFromDataT = 
		std::derived_from<TDerived, TData> || 
		requires(TDerived * _p) { static_cast<TData*>(_p); };

	/// <summary>
	/// TComponent は TData を公開継承している
	/// → ComponentMemento<TComponent, TData> の使用が安全
	/// </summary>
	template <typename TComponent, typename TData>
	concept ComponentFromDataT =
		std::derived_from<TComponent, TData>&&
		ComponentDataT<TData>;

	// ============================================================
	// Memento 関連のコンセプト
	// ============================================================

	class IComponentMemento;

	template <typename T>
	concept MementoT =
		std::derived_from<T, IComponentMemento>;

	/// <summary>
	/// T と M が正しく対応している
	/// </summary>
	template <typename T, typename M>
	concept ComponentWithMementoT =
		// M は IComponentMemento を継承している（memento側の要件）
		std::derived_from<M, IComponentMemento>;

	template <typename T>
	concept StatefulComponentT = 
		requires 
	{
		typename T::Memento;
	}&&
		std::derived_from<typename T::Memento, IComponentMemento>;

	template<class ComponentPoolT, typename TComponent>
	class IComponent;

	template<typename T>
	concept ComponentT = std::derived_from<IComponent<typename T::ComponentPoolT, T>, T>;
		
}