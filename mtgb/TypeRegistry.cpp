#include "TypeRegistry.h"
#include "TypeRegistryImpl.h"
#include "ReflectionInfo.h"
#include "Command.h"
void TypeRegistry::ProvisionalRegister(std::type_index _typeIdx, std::function<void(void)> _registerFunc)
{
	provisionalRegisterFunc_.emplace(_typeIdx, _registerFunc);
}

TypeRegistry& TypeRegistry::Instance()
{
	static TypeRegistry instance;
	return instance;
}

void TypeRegistry::Initialize()
{
	for (auto& itr : provisionalRegisterFunc_)
	{
		itr.second();
	}
}

void TypeRegistry::CallFunc(std::type_index _typeIdx, std::any _instance, const char * _name)
{
	const auto& itr = showFunctions_.find(_typeIdx);
	if (itr != showFunctions_.end())
	{
		Command* command = itr->second(std::any(_instance), _name);
		if (command == nullptr)
			return;
		// ‘€ìƒRƒ}ƒ“ƒh‚ğ“n‚·
		commandListener_(command);
	}
}
bool TypeRegistry::IsRegisteredType(std::type_index _typeIdx)
{
	return showFunctions_.contains(_typeIdx);
}
void TypeRegistry::RegisterCommandListener(std::function<void(Command*)> _commandListenner)
{
	commandListener_ = _commandListenner;
}
TypeRegistry::TypeRegistry()
	: commandListener_{nullptr}
{
}