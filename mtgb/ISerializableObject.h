#pragma once
#include <string>
class ISerializableObject
{
public:
	virtual void OnPreSave() {}
	virtual void OnPostRestore() {}
	static std::string ClassName() { return "Unknown"; }
};
