#pragma once
#include <typeindex>
#include <functional>
#include <unordered_map>
#include <any>
#include <type_traits>
#include "DefaultShow.h"

class Command;

using ShowPropertyFunc = std::function<Command*(std::any, const char*)>;
template <typename Func>
concept ShowPropFuncCallable = std::is_convertible_v<Func, ShowPropertyFunc>;

class PropertyDisplayRegistry
{
  public:
	template <typename T> void RegisterType();
	template <typename T, ShowPropFuncCallable Func> void RegisterFunc(Func&& _func);

	static PropertyDisplayRegistry& Instance();
	// プログラム開始時に登録したい関数を登録
	template <typename Func>
		requires std::is_invocable_v<Func>
	void ProvisionalRegister(std::type_index _typeIdx, Func&& _registerFunc);
	void Initialize();
	template <typename T> void ShowProperty(T* _instance, const char* _name);

	void ShowProperty(std::type_index _typeIdx, std::any _instance, const char* _name);
	bool IsRegisteredType(std::type_index _typeIdx);

	template <typename Func>
		requires std::is_invocable_v<Func, Command*>
	void RegisterCommandListener(Func&& _commandListener);

  private:
	// 型情報をキー、std::functionを値とする
	// 変更があった場合にはその操作をCommandとして返す
	std::unordered_map<std::type_index, ShowPropertyFunc> showFunctions_;
	std::unordered_map<std::type_index, std::function<void(void)>> provisionalRegisterFunc_;

	// 受け取り口へコマンドを登録する関数を持つstd::function
	std::function<void(Command*)> commandListener_;

	template <typename... Args, typename T>
	Command* CheckCustomAttrs(std::tuple<Args...>& _attrs, T _valPtr, const char* _name);

	template <typename T> bool ShowMemberWithReflection(T _memberValue, const char* _name, Command* _command);

	PropertyDisplayRegistry();
	PropertyDisplayRegistry(const PropertyDisplayRegistry&)			   = delete;
	PropertyDisplayRegistry& operator=(const PropertyDisplayRegistry&) = delete;
};
template <typename Func>
	requires std::is_invocable_v<Func>
inline void PropertyDisplayRegistry::ProvisionalRegister(std::type_index _typeIdx, Func&& _registerFunc)
{
	provisionalRegisterFunc_.emplace(_typeIdx, std::forward<Func>(_registerFunc));
}
template <typename T> void PropertyDisplayRegistry::ShowProperty(T* _instance, const char* _name)
{
	Command* command = nullptr;
	const auto& itr	 = showFunctions_.find(typeid(T));
	if (itr != showFunctions_.end())
	{
		command = itr->second(std::any(_instance), _name);
	}
	else
	{
		command = mtgb::DefaultShow(_instance, _name);
	}

	if (command == nullptr)
		return;
	if (commandListener_ == nullptr)
		return;

	// 操作コマンドを渡す
	commandListener_(command);
}

template <typename Func>
	requires std::is_invocable_v<Func, Command*>
inline void PropertyDisplayRegistry::RegisterCommandListener(Func&& _commandListener)
{
	commandListener_ = std::forward<Func>(_commandListener);
}

template <typename T, ShowPropFuncCallable Func> inline void PropertyDisplayRegistry::RegisterFunc(Func&& _func)
{
	using Type = std::remove_cvref_t<T>;
	std::type_index typeIdx(typeid(Type));
	showFunctions_.emplace(typeIdx, std::forward<Func>(_func));
}

namespace RegisterShowFuncHolder
{
	/// <summary>
	/// 型に対応したImGuiの表示処理をセットする
	/// </summary>
	/// <typeparam name="Type">表示したい型</typeparam>
	/// <param name="_func">表示したい型を使った表示関数</param>
	template <typename Type, typename Func>
		requires std::is_invocable_v<Func, Type*, const char*>
	void Set(Func&& _func)
	{
		// TODO : Setに渡した関数自体はCommandとして作られないという説明をするようコメントを更新

		PropertyDisplayRegistry::Instance().RegisterFunc<Type>(
			[=](std::any _target, const char* _name) -> Command*
			{
				_func(std::any_cast<Type*>(_target), _name);
				return nullptr;
			}
		);
	}
}; // namespace RegisterShowFuncHolder

// マクロ定義
#define REGISTER_TYPE(Type, ...)                                              \
	struct Type##_TypeRegister                                                \
	{                                                                         \
		Type##_TypeRegister()                                                 \
		{                                                                     \
			PropertyDisplayRegistry::Instance().ProvisionalRegister(          \
				typeid(Type),                                                 \
				[]()                                                          \
				{                                                             \
					PropertyDisplayRegistry::Instance().RegisterType<Type>(); \
				}                                                             \
			);                                                                \
		}                                                                     \
	};                                                                        \
	static Type##_TypeRegister Type##_instance;                               \
	REFL_TYPE(Type, __VA_ARGS__)

#define REGISTER_FIELD(MemberName, ...) REFL_FIELD(MemberName, __VA_ARGS__)

#define REGISTER_MEMBER_FUNC(MemberName, ...) REFL_FUNC(MemberName, __VA_ARGS__)

#define REGISTER_END REFL_END