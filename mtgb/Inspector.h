#pragma once
#include <typeinfo>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <type_traits>
#include <any>
#include "IncludingWindows.h"
#include "WindowContext.h"
#include "DefaultShow.h"
#include "TypeRegistry.h"

using namespace mtgb;

class Inspector
{
  public:
	static Inspector& Instance();

	template <typename T> void ShowInspector(T* _instance, const char* _name);
	template <typename T> void ShowInspector(const T* _instance, const char* _name);

  private:
	Inspector() {};
	void Show(std::type_index _typeIdx, std::any _instance, const char* _name);
	const static mtgb::WindowContext mainWindow_ = mtgb::WindowContext::FIRST;
};

template <typename T> void Inspector::ShowInspector(T* _instance, const char* _name)
{
	using Type = std::remove_cvref_t<T>;
	std::type_index typeIdx(typeid(Type));
	if (mtgb::CurrContext() != mainWindow_)
	{
		return;
	}

	if (TypeRegistry::Instance().IsRegisteredType(typeIdx))
	{
		TypeRegistry::Instance().CallFunc(typeIdx, std::any(_instance), _name);
	}
	else
	{
		mtgb::DefaultShow(_instance, _name);
	}
}

template <typename T> void Inspector::ShowInspector(const T* _instance, const char* _name)
{
	ShowInspector(const_cast<T*>(_instance), _name);
}
