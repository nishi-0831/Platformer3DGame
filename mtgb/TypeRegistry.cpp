#include "TypeRegistry.h"
#include "TypeRegistryImpl.h"
#include "ReflectionInfo.h"
#include "Command.h"
void PropertyDisplayRegistry::ProvisionalRegister(std::type_index _typeIdx, std::function<void(void)> _registerFunc)
{
	provisionalRegisterFunc_.emplace(_typeIdx, _registerFunc);
}

PropertyDisplayRegistry& PropertyDisplayRegistry::Instance()
{
	static PropertyDisplayRegistry instance;
	return instance;
}

void PropertyDisplayRegistry::Initialize()
{
	for (auto& itr : provisionalRegisterFunc_)
	{
		itr.second();
	}
}

void PropertyDisplayRegistry::ShowProperty(std::type_index _typeIdx, std::any _instance, const char* _name)
{
	const auto& itr = showFunctions_.find(_typeIdx);
	if (itr != showFunctions_.end())
	{
		Command* command = itr->second(std::any(_instance), _name);
		if (command == nullptr)
			return;
		// 操作コマンドを渡す
		commandListener_(command);
	}
}
bool PropertyDisplayRegistry::IsRegisteredType(std::type_index _typeIdx)
{
	return showFunctions_.contains(_typeIdx);
}
void PropertyDisplayRegistry::RegisterCommandListener(std::function<void(Command*)> _commandListenner)
{
	commandListener_ = _commandListenner;
}
PropertyDisplayRegistry::PropertyDisplayRegistry()
	: commandListener_ { nullptr }
{
}