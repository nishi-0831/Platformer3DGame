#include "Inspector.h"

#include "WindowContextUtil.h"

Inspector& Inspector::Instance()
{
	static Inspector instance;
	return instance;
}

void Inspector::Show(std::type_index _typeIdx, std::any _instance, const char* _name)
{
	if (mtgb::WinCtxRes::CurrContext() != mainWindow_)
	{
		return;
	}
	TypeRegistry::Instance().CallFunc(_typeIdx, _instance, _name);
}