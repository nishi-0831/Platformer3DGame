#include "Debug.h"
#include "..\ImGui\imgui.h"

mtgb::Debug::Debug()
{
}

mtgb::Debug::~Debug()
{
}

void mtgb::Debug::Initialize()
{
}

void mtgb::Debug::Update()
{
	if (logs_.size() > MAX_LOG_COUNT)
	{
		for (LogItr itr = logs_.begin(); itr != std::next(logs_.begin(), logs_.size() - MAX_LOG_COUNT);)
		{
			itr = RemoveLog(itr);
		}
	}
}

std::list<mtgb::LogEntry> mtgb::Debug::GetLog()
{
	return logs_;
}

mtgb::Debug::LogItr mtgb::Debug::RemoveLog(LogItr _itr)
{
	logMap_.erase(_itr->msgDetail);
	return logs_.erase(_itr);
}

std::string mtgb::Debug::MakeKey(
	const std::string& _object,
	const char* _file,
	int _line,
	const char* _func,
	const std::string _msg
)
{
	return _msg + "|" + _object + "|" + _file + "|" + std::to_string(_line) + _func;
}
