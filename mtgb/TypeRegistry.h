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
	template<typename T>
	void RegisterType();
	
	template<typename T>
	void RegisterFunc(std::function<Command*(std::any, const char*)> func);

	static TypeRegistry& Instance();
	// プログラム開始時に登録したい関数を登録
	// マクロで登録していると初期化のタイミングが制御できない
	void ProvisionalRegister(std::type_index typeIdx, std::function<void(void)> registerFunc);
	void Initialize();
	template<typename T>
	void CallFunc(T* instance, const char* name);

	void CallFunc(std::type_index typeIdx, std::any instance, const char* name);
	bool IsRegisteredType(std::type_index typeIdx);

	void RegisterCommandListener(std::function<void(Command*)> _commandListener);

private:
	// 型情報をキー、std::functionを値とする
	// 変更があった場合にはその操作をCommandとして返す
	std::unordered_map<std::type_index, std::function<Command*(std::any, const char*)>> showFunctions_;
	std::unordered_map<std::type_index, std::function<void(void)>> provisionalRegisterFunc_;

	// 受け取り口へコマンドを登録する関数を持つstd::function
	std::function<void(Command*)> commandListener_;

	template<typename... Args, typename T>
	Command* CheckCustomAttrs(std::tuple<Args...>& attrs, T valPtr, const char* name);
	
	template<typename T>
	void CheckProxyAttrs();

	

	template<typename T>
	bool ShowMemberWithReflection(T memberValue, const char* name, Command* command);

	TypeRegistry();
	TypeRegistry(const TypeRegistry&) = delete;
	TypeRegistry& operator=(const TypeRegistry&) = delete;
};
template<typename T>
void TypeRegistry::CallFunc(T* instance, const char* name)
{
	Command* command = nullptr;
	const auto& itr = showFunctions_.find(typeid(T));
	if (itr != showFunctions_.end())
	{
		command = itr->second(std::any(instance),name);
	}
	else
	{
		command = mtgb::DefaultShow(instance, name);
	}

	if (command == nullptr)
		return;
	if (commandListener_ == nullptr)
		return;

	// 操作コマンドを渡す
	commandListener_(command);
}

template<typename T>
void TypeRegistry::RegisterFunc(std::function<Command* (std::any, const char*)> func)
{
	using Type = std::remove_cvref_t<T>;
	std::type_index typeIdx(typeid(Type));
	showFunctions_[typeIdx] = func;
}
namespace RegisterShowFuncHolder
{
	/// <summary>
	/// 型に対応したImGuiの表示処理をセットする
	/// </summary>
	/// <typeparam name="Type">表示したい型</typeparam>
	/// <param name="_func">表示したい型を使った表示関数</param>
	template<typename Type>
	void Set(std::function<void(Type* _target, const char* _name)> _func)
	{
		//TODO : Setに渡した関数自体はCommandとして作られないという説明をするようコメントを更新

		TypeRegistry::Instance().RegisterFunc<Type>([=](std::any target, const char* name) -> Command*
			{
				_func(std::any_cast<Type*>(target), name);
				return nullptr;
			});
	}
};

// マクロ定義
#define REGISTER_TYPE(Type, ...) \
struct Type##_TypeRegister{ \
	Type##_TypeRegister(){\
		TypeRegistry::Instance().ProvisionalRegister(typeid(Type),[](){TypeRegistry::Instance().RegisterType<Type>();});\
	}\
};\
static Type##_TypeRegister Type##_instance;\
REFL_TYPE(Type, __VA_ARGS__)

#define REGISTER_FIELD(MemberName,...)\
REFL_FIELD(MemberName,__VA_ARGS__)

#define REGISTER_MEMBER_FUNC(MemberName,...)\
REFL_FUNC(MemberName,__VA_ARGS__)

#define REGISTER_END REFL_END