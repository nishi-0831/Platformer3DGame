#pragma once
#include <concepts>
#include <type_traits>

namespace mtgb
{
	// ============================================================
	// コンポーネント関連のコンセプト
	// ============================================================

	class IComponentMemento;

	// SaveToMemento関数が実装されている
	template <typename T>
	concept HasSaveToMementoMethod = requires(T _t) {
		// SaveToMementoという名前の関数を呼び出すことができる
		{ _t.SaveToMemento() };
		// 戻り値の型はIComponentMementoを継承している
	} && std::derived_from<std::remove_pointer_t<decltype(std::declval<T>().SaveToMemento())>, IComponentMemento>;

	// RestoreFromMemento関数が実装されている
	template <typename T>
	concept HasRestoreFromMementoMethod = requires(T _t, const T::Memento& _m) {
		{ _t.RestoreFromMemento(_m) } -> std::same_as<void>;
	};

	// 前方宣言
	template <class ComponentPoolT, typename TComponent> class IComponent;

	// IComponentを継承している
	template <typename T>
	concept DerivedIComponent = std::derived_from<T, IComponent<typename T::Pool, T>>;

	// IComponentを継承していて、Memento操作機能を持つ
	template <typename T>
	concept IComponentWithMemento = DerivedIComponent<T> && HasSaveToMementoMethod<T> && HasRestoreFromMementoMethod<T>;
} // namespace mtgb