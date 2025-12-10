#pragma once
class ISerializableObject
{
public:
	virtual void OnPreSave() {}
	virtual void OnPostRestore() {}
};
