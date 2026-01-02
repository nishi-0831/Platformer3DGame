#pragma once
#include <typeindex>
#include <functional>
#include <unordered_map>
#include <any>
#include "DefaultShow.h"

class Command;
class TypeRegistry
{
  public:
	template <typename T> void RegisterType();

	template <typename T> void RegisterFunc(std::function<Command*(std::any, const char*)> _func);

	static TypeRegistry& Instance();
	// プログラム開始時に登録したい関数を登録
	void ProvisionalRegister(std::type_index _typeIdx, std::function<void(void)> _registerFunc);
	void Initialize();
	template <typename T> void CallFunc(T* _instance, const char* _name);

	void CallFunc(std::type_index _typeIdx, std::any _instance, const char* _name);
	bool IsRegisteredType(std::type_index _typeIdx);

	void RegisterCommandListener(std::function<void(Command*)> _commandListener);

  private:
	// 型情報をキー、std::functionを値とする
	// 変更があった場合にはその操作をCommandとして返す
	std::unordered_map<std::type_index, std::function<Command*(std::any, const char*)>> showFunctions_;
	std::unordered_map<std::type_index, std::function<void(void)>> provisionalRegisterFunc_;

	// 受け取り口へコマンドを登録する関数を持つstd::function
	std::function<void(Command*)> commandListener_;

	template <typename... Args, typename T>
	Command* CheckCustomAttrs(std::tuple<Args...>& _attrs, T _valPtr, const char* _name);

	template <typename T> void CheckProxyAttrs();

	template <typename T> bool ShowMemberWithReflection(T _memberValue, const char* _name, Command* _command);

	TypeRegistry();
	TypeRegistry(const TypeRegistry&)			 = delete;
	TypeRegistry& operator=(const TypeRegistry&) = delete;
};
template <typename T> void TypeRegistry::CallFunc(T* _instance, const char* _name)
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

template <typename T> void TypeRegistry::RegisterFunc(std::function<Command*(std::any, const char*)> _func)
{
	using Type = std::remove_cvref_t<T>;
	std::type_index typeIdx(typeid(Type));
	showFunctions_[typeIdx] = _func;
}
namespace RegisterShowFuncHolder
{
	/// <summary>
	/// 型に対応したImGuiの表示処理をセットする
	/// </summary>
	/// <typeparam name="Type">表示したい型</typeparam>
	/// <param name="_func">表示したい型を使った表示関数</param>
	template <typename Type> void Set(std::function<void(Type* _target, const char* _name)> _func)
	{
		// TODO : Setに渡した関数自体はCommandとして作られないという説明をするようコメントを更新

		TypeRegistry::Instance().RegisterFunc<Type>(
			[=](std::any target, const char* name) -> Command*
			{
				_func(std::any_cast<Type*>(target), name);
				return nullptr;
			}
		);
	}
}; // namespace RegisterShowFuncHolder

// マクロ定義
#define REGISTER_TYPE(Type, ...)                                   \
	struct Type##_TypeRegister                                     \
	{                                                              \
		Type##_TypeRegister()                                      \
		{                                                          \
			TypeRegistry::Instance().ProvisionalRegister(          \
				typeid(Type),                                      \
				[]()                                               \
				{                                                  \
					TypeRegistry::Instance().RegisterType<Type>(); \
				}                                                  \
			);                                                     \
		}                                                          \
	};                                                             \
	static Type##_TypeRegister Type##_instance;                    \
	REFL_TYPE(Type, __VA_ARGS__)

#define REGISTER_FIELD(MemberName, ...) REFL_FIELD(MemberName, __VA_ARGS__)

#define REGISTER_MEMBER_FUNC(MemberName, ...) REFL_FUNC(MemberName, __VA_ARGS__)

#define REGISTER_END REFL_END