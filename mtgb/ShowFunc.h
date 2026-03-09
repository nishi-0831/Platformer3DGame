#pragma once
#include <type_traits>

#include "Command.h"
template <typename Func>
struct ShowFunc : refl::attr::usage::type
{
  private:
	Func func;

  public:
	constexpr ShowFunc(Func _func)
		: func(_func)
	{
	}
	template <typename T> Command* operator()(T _value, const char* _name) const
	{
		return func(_value, _name);
	}
};

// ShowFuncを作成するヘルパー関数
template <typename F> constexpr auto make_show_func(F&& _f)
{
	return ShowFunc<F>(std::forward<F>(_f));
}